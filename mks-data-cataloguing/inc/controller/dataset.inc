<?php
require_once (dirname ( __FILE__ ) . '/abstract-controller.inc');
class MKSDC_Dataset_Controller extends MKSDC_AbstractController {
	public function __construct($args) {
		if (session_id () == '') {
			session_start ();
		}
		parent::__construct ( $args );
	}
	
	private function _checkLoggedIn() {
		if (! is_user_logged_in ()) {
			auth_redirect ();
		}
		// XXX In any case force authentication
		if (class_exists ( 'phpCAS' )) {
			phpCAS::forceAuthentication ();
		}
	}
	
	private function _userCanEdit() {
		// XXX Get the post author and check whether it is the same as the user id
		$post = get_post ();
		$post_author = $post->post_author;
		if (get_current_user_id () == $post_author) {
			return true;
		}
		return false;
	}
	
	public function indexAction() {
		
		// If this is not a dataset page, then redirect to create a new dataset!
		if(get_post_type() != 'mksdc-datasets'){
			$this->_new();
			return;
		}
		
		$db = MKSDC_DataAccess::instance ();
		$model = array ();
		$model = $db->getDataset ( get_the_ID () );
		$model ['single'] = TRUE;
		$v = new MVC_Viewer ( $model );
		$v->render ( 'dataset-view' );
	}
	
	private function _new() {
		// Only for logged in users
		$this->_checkLoggedIn ();
		// Any logged user can create a dataset
		// If method is GET, we go showing the form
		if ($_SERVER ['REQUEST_METHOD'] == 'GET') {
			$this->_newGET ();
		} else {
			// We try saving the data
			$this->_newPOST ();
		}
	}
	
	private function _newGET() {
		// Only for logged in users
		$this->_checkLoggedIn ();
		// Any logged user can prepare a new dataset
		
		// Prepare a new empty Dataset
		$model = array();
		$model['licence'] = array();
		$view = new MVC_Html($model);
		$view->render('new-dataset');
	}
	
	private function _newPOST() {
		
		// Only for logged in users
		$this->_checkLoggedIn ();
		
		// Any logged user can post a dataset
		// XXX Test WP Nonce
		if( ! wp_verify_nonce(@$_POST['_wpnonce'], 'new-dataset_' . get_current_user_id()) ){
			$this->_badRequest('Nonce verification failed.');
			return;
		}
		
		$errors = $this->validate($this->_validationRules());
		if(count($errors)){
			// Validation failed ...
			$model = $this->_prepare_post_data_to_form_input();
			// XXX Add error messages
			$model['_errors'] = $errors;
			$v = new MVC_Html ( $model );
			$v->render ( 'new-dataset' );
			return;
		}
		
		// Prepare the data
		$data = $this->_prepare_post_to_db();// Id is 0
		
		//
		$db = MKSDC_DataAccess::instance ();
		$newId = $db->createDataset ( $data );
		if ($newId) {
			// Success, redirect to dataset page
			wp_redirect ( get_permalink ( $newId ), 303 );
			exit ();
		} else {
			// An error occurred
			$this->_error ();
		}
	}
	
	public function editAction() {
		// Only for logged in users
		$this->_checkLoggedIn ();
		if(!$this->_userCanEdit()){
			// Cannot Edit
			$this->_forbidden('Cannot edit the dataset');
			exit();
		}
		
		// If method is GET, we go showing the form
		if ($_SERVER ['REQUEST_METHOD'] == 'GET') {
			$this->_editGET ();
		} else {
			// We try saving the data
			$this->_editPOST ();
		}
	}
	
	private function _editGET() {
		$model = $this->_prepare_db_to_form_input();
		$v = new MVC_Html ( $model );
		$v->render ( 'edit-dataset' );
	}
	
	private function _validationRules(){
		// TODO Check file types
		return array('mksdc_nds_title' => array('NotNull','NotEmpty'),
				'mksdc_nds_description' => array('NotNull','NotEmpty'),
				'mksdc_nds_category' => array('NotNull','NotEmpty'),
				'mksdc_nds_tags' => array('NotNull','NotEmpty'),
				'mksdc_sources_field' => array(new MKSDC_Sources_Validator()));
	}
	private function _editPOST() {
		// XXX Test WP Nonce
		if(!wp_verify_nonce(@$_POST['_wpnonce'], 'edit-dataset_' . get_the_ID() )){
			$this->_badRequest('Nonce verification failed.');
			return;	
		}
		
		// XXX Input Validation
		$errors = $this->validate($this->_validationRules());
		if(count($errors)){
		//	print_r($errors);exit();
			// Validation failed ...
			$model = $this->_prepare_post_data_to_form_input();
			$model['_errors'] = $errors;
			// XXX Add error messages
			$v = new MVC_Html ( $model );
			$v->render ( 'edit-dataset' );
			return;
		}
		// Prepare the data
		$data = $this->_prepare_post_to_db(get_the_ID());
		$db = MKSDC_DataAccess::instance ();
		if ($db->updateDataset ( get_the_ID (), $data ) == get_the_ID ()) {
			// Success, redirect to dataset page
			wp_redirect ( get_permalink ( get_the_ID () ), 303 );
			exit ();
		} else {
			// An error occurred
			$this->_error ();
		}
	}
	
	private function _prepare_post_data_to_form_input(){
		$data = $_POST;
		$data['mksdc_nds_category'] = array();
		if(is_array(@$_POST['mksdc_nds_category'])){
			foreach($_POST['mksdc_nds_category'] as $i){
				array_push($data['mksdc_nds_category'], array('id'=>htmlspecialchars($i)));
			}
		}
		$data['mksdc_nds_tags']= array();
		if(is_array(@$_POST['mksdc_nds_tags'])){
			foreach($_POST['mksdc_nds_tags'] as $i){
				array_push($data['mksdc_nds_tags'], array('id'=>htmlspecialchars($i)));
			}
		}
		$data['mksdc_nds_formats'] = array();
		if(is_array(@$_POST['mksdc_nds_formats'])){
			foreach($_POST['mksdc_nds_formats'] as $i){
				array_push($data['mksdc_nds_formats'], array('id'=>htmlspecialchars($i)));
			}
		}
		$data['mksdc_nds_owners'] = array();
		if(is_array(@$_POST['mksdc_nds_owners'])){
			foreach($_POST['mksdc_nds_owners'] as $i){
				array_push($data['mksdc_nds_owners'], array('name'=>htmlspecialchars($i),'id'=>htmlspecialchars($i)));
			}
		}
		
		if(isset($_FILES['mksdc_sources_field'])){
			foreach($_FILES['mksdc_sources_field']['name'] as $ix => $value){
				$data['mksdc_sources_field'][$ix]['file'] = $value['file']; // Just show the name.
			}	
		}
//		print_r($data); die;
		return $data;
	}
	
	private function _prepare_db_to_form_input(){
		$db = MKSDC_DataAccess::instance ();
		$model = array ();
		// TODO Implement extension to contribute alternative edit page?
		$model = $db->getDataset ( get_the_ID () );
		// Setup form fields
		$model ['mksdc_nds_title'] = $model ['post']->post_title;
		$model ['mksdc_nds_description'] = $model ['content'];
		$model ['mksdc_nds_category'] = array();
		foreach($model['categories'] as $cat){
			array_push($model ['mksdc_nds_category'], array('id'=>$cat->cat_ID,'name'=>$cat->name));
		}
		$model['mksdc_nds_tags'] = array();
		if(is_array(@$model['tags'])){
			foreach($model['tags'] as $cat){
					array_push($model ['mksdc_nds_tags'], array('id'=>$cat->name,'name'=>$cat->name));
			}
		}
		
		$model['mksdc_nds_formats'] = array();
		if(is_array(@$model['formats'])){
			foreach($model['formats'] as $frmt){
				array_push($model ['mksdc_nds_formats'], array('id'=>$frmt->term_id,'name'=>$frmt->name));
			}
		}
		$model['mksdc_nds_owners'] = array();
		if(is_array(@$model['owner'])){
			foreach($model['owner'] as $frmt){
				array_push($model ['mksdc_nds_owners'], array('id'=>$frmt->name,'name'=>$frmt->name));
			}
		}
		$model['mksdc_nds_source_label'] = $model['source']['label'];
		$model['mksdc_nds_source_link'] = $model['source']['link'];
		$model ['mksdc_nds_attribution'] = $model ['attribution'];
		$model['mksdc_services_field'] = $model ['services'];
		$model['mksdc_sources_field'] = $model['sources'];
		return $model;
	}
	
	private function _prepare_post_to_db($post_id = 0){
		
		$data = $_POST;
		// XXX This is all about overriding some of the form fields.
		// Plugins that contribute tabs should then implement their own saving process.
		// WP POST
		$data ['post_ID'] = $post_id;
		$data ['post_type'] = 'mksdc-datasets';
		$data ['post_status'] = 'publish';
		// Tab Information
		// Title and Description
		$data ['post_title'] = $_POST ['mksdc_nds_title'];
		$data ['post_content'] = $_POST ['mksdc_nds_description'];
		// Categories (array of ids)
		$data ['post_category'] = @$_POST ['mksdc_nds_category'];
		// Taxonomies
		$data ['tax_input'] = array ();
		$data ['tax_input'] ['post_tag'] = @$_POST ['mksdc_nds_tags'];
		$data ['tax_input'] ['mksdc-dataformats'] = (@$_POST ['mksdc_nds_formats'])?$_POST ['mksdc_nds_formats']:array();
		// Tab Ownership and Licence
		$data ['tax_input'] ['mksdc-dataowners'] = @$_POST ['mksdc_nds_owners'];
		$data ['mksdc_link_to_source_desc_field'] = $_POST ['mksdc_nds_source_label'];
		$data ['mksdc_link_to_source_field'] = $_POST ['mksdc_nds_source_link'];
		// Licenses
		if(isset($_POST ['mksdc_nds_licence'])){
			$lcounter = 1;
			foreach ( @$_POST ['mksdc_nds_licence'] as $licencePostId ) {
				$data ['mksdc_license_' . $lcounter . '_field'] = $licencePostId;
				$lcounter ++;
			}
		}
		$data['mksdc_attribution_field'] = $_POST['mksdc_nds_attribution'];
		
		// Prepare Sources
		$data['mksdc_sources_field'] = array();
		$files_keys = array('name','type','tmp_name','error','size');
		if(isset($_POST['mksdc_sources_field'] )){
			foreach($_POST['mksdc_sources_field'] as $index => $source){
				foreach($files_keys as $key){
					if(isset($_FILES['mksdc_sources_field'][$key][$index])){
						$source['file'][$key] = $_FILES['mksdc_sources_field'][$key][$index]['file'];
					}
				}
				$data['mksdc_sources_field'][$index] = $source;
			}
		}
		return $data;
	}
}
class MKSDC_Sources_Validator implements MVC_List_Validator{
	private $_types;
	public function __construct(){
		$this->_types = get_allowed_mime_types();
	}
	public function test($val){
		$errors = array();
		if(isset($_FILES['mksdc_sources_field'])){
			foreach($_FILES['mksdc_sources_field'] as $k => $f){
				foreach($f as $f1){
					if($k == 'type'){
						$valid = in_array($f1['file'], $this->_types);
						if(!$valid){
							$errors[$k] = 'File type is not supported: ' . $f1['file'];
						}
					}
				}
			}
		}
		return (count($errors) > 0) ? $errors : TRUE;
	}
}
$c = new MKSDC_Dataset_Controller ( array (
		'action_parameter' => 'action',
		'parameters' => array_merge($_POST, $_GET)
) );
$c->run ();
