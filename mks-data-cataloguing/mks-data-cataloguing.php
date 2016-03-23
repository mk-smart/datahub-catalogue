<?php
/**
 * @package MKS Data Cataloguing
 */
/*
 Plugin Name: MKS Data Cataloguing
Plugin URI: http://mksmart.org
Description: To register datasets and the information related to their import and redistribution in the MK:Smart Datahub
Version: 1.2-RC1 - 23-03-2016
Author: enridaga, mdaquin
Author URI: http://kmi.open.ac.uk/
License: Apache 2.0
*/

// send message when called directly
if ( !function_exists( 'add_action' ) ) {
	echo 'This is a plugin... what do you want?';
	exit;
}

// Activate log only if WP_DEBUG is true
define('MKSDC_LOG', WP_DEBUG);

require_once(dirname(__FILE__) . '/inc/mvctools.inc');
require_once(dirname(__FILE__) . '/inc/dataaccess.inc');

class MKSDC_Plugin{

	/**
	 * A reference to an instance of this class.
	 */
	private static $_instance;

	/**
	 * The array of templates that this plugin tracks.
	 */
	protected $_templates;

	/**
	 * Extension points
	 * 
	 * @var unknown
	 */
	protected $_extensions;

	const DATA_CATALOGUE_TMPL = 'inc/controller/data-catalogue.inc';
	const DATASET_TMPL = 'inc/controller/dataset.inc';
	const API_TMPL = 'inc/controller/api.inc';
	
	/**
	 * Messages are logged only if in debug mode
	 */
	public static function log(){
		if(MKSDC_LOG === true){
			error_log("MKSDC " . implode(' ', func_get_args()));
		}
	}
	
	public static function templateDirectory(){
		return dirname(__FILE__) . '/inc/html';
	}
	
	/**
	 * Errors are always logged
	 * 
	 * @param string $m
	 * @param Exception $e
	 */
	public static function logErr($m, Exception $e = NULL){
		if(MKSDC_LOG === true){
			$m = "Error " . $m;
			if($e != NULL){
				$m .= " :: " . $e->getMessage() . " \n" . $e->getTraceAsString();
			}
			error_log("MKSDC " . $m);
		}
	}

	public static function getRDFCatUrl(){
		return get_option('mksdc_rdfcat_url');
	}

	public static function getRDFCatApiKey(){
		return get_option('mksdc_rdfcat_key');
	}
	
	public static function getLinkedDataURI($local=""){
		return "http://datahub.mksmart.org/ns/$local";
	}
	
	public static function getPostFromLinkedDataURI($uri){
		if(strpos($uri, self::getLinkedDataURI('dataset/')) === 0 ){
			// this is a dataset
			$post_name = substr($uri, strlen(self::getLinkedDataURI('dataset/')));
			$post = get_page_by_path($post_name, OBJECT ,'mksdc-datasets');
			return $post;
		}else{
			return NULL;
		}
	}

	public static function getDatasetLink($post_id, array $otherParams = array()){
		return self::extendLink(get_permalink($post_id), $otherParams);
	}

	public static function getDataCatalogueLink(array $params = array()){
		return self::getPageFromTemplate(self::DATA_CATALOGUE_TMPL, $params);
	}
	
	public static function getDatasetApiLink($name){
		$params = array('action' => 'dataset', 'name'=> $name);
		return self::getApiLink($params);
	}
	
	public static function getApiLink(array $params = array()){
		return self::getPageFromTemplate(self::API_TMPL, $params);
	}
	
	private static function extendLink($link, array $qsparams = array()){
		if(count($qsparams) == 0){
			return $link;
		}
		if(strpos($link, '?')!==FALSE){
			$link = $link . '&';
		}else{
			$link = $link . '?';
		}
		
		$pars = array();
		foreach($qsparams as $par => $val){
			array_push($pars, $par . '=' . urlencode($val));
		}
		$querystring = join('&', $pars);
		return $link . $querystring;
	}
	
	public static function getPageFromTemplate($tmpl, array $qsparams = array()){
		$pages = get_pages(array(
				'meta_key' => '_wp_page_template',
				'meta_value' => $tmpl
		));
		$link = NULL;
		foreach($pages as $afp){
			$link = get_page_link($afp->ID);
			break;
		}
		
		if($link == NULL){
			return FALSE;
		}
		
		return self::extendLink($link, $qsparams);
	}
	
	public function __construct(){
		add_action('init', array($this, 'init'));
		add_filter('upload_mimes', array($this, 'registerCustomMediaTypes'));
	}

	// Init: create taxonomies and custom post type
	public function init() {
		try{
			
			// Include Settings for admin panel
			include(dirname(__FILE__) . '/inc/settings.inc' );
			
			// Include custom data items, UI objects and save action
			require_once dirname(__FILE__) . '/inc/custom-data.inc';

			// Register taconomies
			require_once dirname(__FILE__) . '/inc/taxonomies.inc';

			// Register custom post type Dataset
			require_once dirname(__FILE__) . '/inc/post-types.inc';

			// Linked Data utilities
			require_once dirname(__FILE__) . '/inc/linked-data.inc';

			// Activate RDF catalogue notifier
			require_once dirname(__FILE__) . '/inc/rdf-catalogue-notifier.inc';
				
			// Register with hook 'wp_enqueue_scripts', which can be used for front end CSS and JavaScript
			add_action( 'wp_print_scripts', array($this, 'add_scripts' ));

			// Register custom shortcodes
			require_once dirname(__FILE__) . '/inc/shortcodes.inc';
				
			// Init page templates
			$this->_initPageTemplates();

		}catch(Exception $e){
			self::logErr("Error on Plugin initialization", $e);
		}
	}

	/**
	 * Enqueue plugin styles/scripts
	 */
	function add_scripts(){
		// Bootstrap
		wp_register_script('mksdc-js', plugins_url('/js/mksdc.js', __FILE__), array('jquery'));
		wp_register_style('mksdc-style', plugins_url('/css/mksdc.css', __FILE__));
		wp_register_script('select2-js', '//cdnjs.cloudflare.com/ajax/libs/select2/4.0.0/js/select2.min.js', array('jquery'));
		wp_register_style('select2-css', '//cdnjs.cloudflare.com/ajax/libs/select2/4.0.0/css/select2.min.css');
		wp_enqueue_script('mksdc-js');
		wp_enqueue_style('mksdc-style');
		wp_enqueue_script('select2-js');
		wp_enqueue_style('select2-css');
	}

	/**
	 * Initialize page templates
	 * 
	 */
	private function _initPageTemplates(){
		$this->_templates = array();
		// Add a filter to the attributes metabox to inject template into the cache.
		add_filter( 'page_attributes_dropdown_pages_args', array( $this, 'registerProjectTemplates' ) );
		// Add a filter to the save post to inject out template into the page cache
		add_filter( 'wp_insert_post_data', array( $this, 'registerProjectTemplates' ) );
		// Add a filter to the template include to determine if the page has our
		// template assigned and return it's path
		add_filter( 'template_include', array( $this, 'viewProjectTemplate' ) );
		// Add your templates to this array.
		$this->_templates = array(
			self::DATA_CATALOGUE_TMPL => 'Data Catalogue Public Page',
			self::API_TMPL => 'Data Catalogue Api',
			self::DATASET_TMPL => 'New Dataset Page',
		);
	}
	
	public function registerCustomMediaTypes( $existing_mimes = array()){
		// Added to extend allowed files types in Media upload
		$existing_mimes['csv'] = 'text/csv';
		$existing_mimes['xml'] = 'text/xml';
		$existing_mimes['json'] = 'application/json';
		$existing_mimes['rdf'] = 'application/rdf+xml';
		$existing_mimes['ttl'] = 'text/turtle';
		$existing_mimes['nt'] = 'text/plain';
		$existing_mimes['n3'] = 'text/rdf+n3';
		$existing_mimes['ld'] = 'application/ld+json';
		
		return $existing_mimes;
	}

	/**
	 * Adds our template to the pages cache in order to trick WordPress
	 * into thinking the template file exists where it doens't really exist.
	 */
	public function registerProjectTemplates( $atts ) {
		// Create the key used for the themes cache
		$cache_key = 'page_templates-' . md5( get_theme_root() . '/' . get_stylesheet() );
		// Retrieve the cache list.
		// If it doesn't exist, or it's empty prepare an array
		$templates = wp_get_theme()->get_page_templates();
		if ( empty( $templates ) ) {
			$templates = array();
		}
		// New cache, therefore remove the old one
		wp_cache_delete( $cache_key , 'themes');
		// Now add our template to the list of templates by merging our templates
		// with the existing templates array from the cache.
		$templates = array_merge( $templates, $this->_templates );
		// Add the modified cache to allow WordPress to pick it up for listing
		// available templates
		wp_cache_add( $cache_key, $templates, 'themes', 1800 );
		return $atts;
	}

	/**
	 * Checks if the template is assigned to the page
	 */
	public function viewProjectTemplate( $template ) {
		global $post;

		if ((!$post) || !isset($this->_templates[get_post_meta($post->ID, '_wp_page_template', true)] ) ) {
			return $template;
		}
		$file = plugin_dir_path(__FILE__). get_post_meta(
				$post->ID, '_wp_page_template', true
		);
		// Just to be safe, we check if the file exist first
		if( file_exists( $file ) ) {
			return $file;
		} else {
			MKSDC_Plugin::logErr('Template not found: ' . $file);
		}
		return $template;

	}

	/**
	 * Returns an instance of this class.
	 */
	public static function instance() {
		if( null == self::$_instance ) {
			self::$_instance = new MKSDC_Plugin();
		}
		return self::$_instance;
	}

	/**
	 * Available points are:
	 * <ul>
	 * <li> dataset : to add data to a dataset description
	 * <li> service : to add a service to a dataset description
	 * <li> private : to provide a list of private datasets (UUIDs)
	 * <li> dataset-render : to contribute to the display of a dataset
	 * <li> dataset-edit-tab: to contribute a new tab to the edit dataset page
	 * <li> dataset-edit-tab-content : to contribute a panel to the edit dataset page
	 * </ul>
	 * 
	 * Ext must be a function name or an array(obj,methodName):
	 * <ul>
	 * <li> dataset : expecting the dataset post_id as argument 
	 * <li> service : expecting the dataset post_id as argument
	 * <li> private : no parameter expected
	 * <li> dataset-render : expects dataset data as input, prints some HTML that is injected in the dataset view.
	 * <li> dataset-edit-tab : no parameter is expected. 
	 * Must return an array of three elements: [0] tab id [1] tab label [2] glyphicon icon name
	 * <li> dataset-edit-tab-content : expects a tabId as first parameter and the dataset data as second. Prints HTML to be injected in the edit dataset page.
	 * </ul>
	 * 
	 * @param string $point - functionality to extend
	 * @param string $name - id of the exntension
	 * @param mixed $ext - function to run or array($obj,$methodName)
	 */
	public function registerExtension($point, $name, $ext){ 
		$this->_extensions[$point][$name] = $ext;
	}
	
	public function unregisterExtension($point, $name){
		unset($this->_extensions[$point][$name]);
	}
	
	public function getExtensions($point){
		if(isset($this->_extensions[$point])){
			return $this->_extensions[$point];
		}else{
			return array();
		}
	}
}
add_action('plugins_loaded', array('MKSDC_Plugin', 'instance'));
