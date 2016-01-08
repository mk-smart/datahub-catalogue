<?php
/**
 * Data access layer of MKSDC Plugin.
 * This class is a singleton.
 * <pre>
 * $db = MKSDC_DataAccess::instance();
 * </pre>
 * @author enridaga
 *
 */
class MKSDC_DataAccess{

	private static $_instance = NULL;

	/**
	 * List of licenses (aka redistribution policies) from WP
	 *
	 * @return array - Associative array. Key: Post-Id; Value: Post-Title
	 */
	public function getLicensesSelectOptions(){
		// Prepare options once for all
		$options = array();
		$args=array('post_type' => 'mksdc-policies');
		$my_query = new WP_Query($args);
		if( $my_query->have_posts() ) {
			while ($my_query->have_posts()) {
				$my_query->the_post();
				$options[get_the_ID()]= get_the_title();
			}
		}
		uasort($options, 'mksdc_sorter_policy_options');
		wp_reset_query();
		return $options;
	}

	public function getUserId(){
		return get_current_user_id();
	}

	public function getUsername(){
		$user = wp_get_current_user();
		return $user->get('user_login');
	}

	public static function toKey($name){
		return '_mksdc_' . trim($name) . '_key';
	}

	public static function toField($name){
		return 'mksdc_' . trim($name) . '_field';
	}

	/**
	 *
	 * @param array $data - array of fields
	 * @param string $name - The name of the field without prefix and suffix (eg: 'attribute' for 'mksdc_attribute_field')
	 */
	public function hasField($name, array $data){
		return isset($data[self::toField($name)]);
	}

	/**
	 *
	 * @param int $post_id
	 * @param string $name
	 */
	public function readCustomField($post_id, $name){
		return get_post_meta($post_id, self::toKey($name), true);
	}

	/**
	 *
	 * @param int $post_id
	 * @param string $name
	 */
	public function readCustomFieldArray($post_id, $name){
		return unserialize(get_post_meta($post_id, self::toKey($name), true));
	}

	/**
	 * Applies sanitation on the value.
	 *
	 * @param int $post_id
	 * @param string $name - The name of the field without prefix and suffix (eg: 'attribute' for 'mksdc_attribute_field')
	 * @param array $data - An associative array having the full field name as one of the key.
	 */
	public function saveCustomField($post_id, $name, array $data){
		$key = self::toField($name);
		if (isset($data[$key])) {
			self::saveMetaField($post_id, $name, sanitize_text_field($data[$key]));
		}
	}

	/**
	 * Saves data in a Wordpress Meta Field.
	 *
	 * @param unknown $post_id
	 * @param unknown $name - The name of the field without prefix and suffix (eg: 'attribute' for 'mksdc_attribute_field')
	 * @param unknown $value - The raw value to be saved
	 */
	public function saveMetaField($post_id, $name, $value){
		update_post_meta( $post_id, self::toKey($name), $value);
	}

	/**
	 * Saves data in a Wordpress Meta Field.
	 *
	 * To save an array in a single custom field (serialized)
	 *
	 * @param int $post_id
	 * @param string $name - The name of the field without prefix and suffix (eg: 'attribute' for 'mksdc_attribute_field')
	 * @param array $data - An associative array having the full field name as one of the key.
	 */
	public function saveCustomFieldArray($post_id, $name, array $data){
		$key = self::toField($name);
		if (isset($data[$key])) {
			self::saveMetaField($post_id, $name, serialize($data[$key]));
		}
	}

	/**
	 *
	 * Saves data in a Wordpress Meta Field.
	 *
	 * To save a list of values in different indexed custom fields.
	 *
	 * @param int $post_id
	 * @param string $name
	 * @param array $data
	 */
	public function saveCustomFieldList($post_id, $name, array $data){
		$i = 1;
		$values=array();
		while( isset($data[self::toField($name . '_' . $i)]) ){
			$v=sanitize_text_field($data[self::toField($name . '_' . $i)]);
			if($v && !in_array($v, $values)){
				array_push($values, $v);
			}
			$i++;
		}
		$deleteupto = $i;
		array_unique($values, SORT_ASC );
		$i = 1;
		foreach( $values as $lic){
			self::saveMetaField($post_id, $name . '_' . $i, $lic );
			$i++;
		}
		while($i<=$deleteupto){
			delete_post_meta($post_id, self::toKey($name . '_' . $i) );
			$i++;
		}
	}

	/**
	 * Reads data in a Wordpress Meta Field.
	 *
	 * Data is casted to float. If there is no data, FALSE is returned;
	 *
	 * @param int $post_id
	 * @param string $name
	 * @param $dateFormat - If date format is provided, applies gmdate.
	 * @return multitype:
	 */
	public function readCustomFieldTimestamp($post_id, $name, $dateFormat = FALSE){
		$v = $this->readCustomField($post_id, $name);
		if( $v !== '' ){
			if($dateFormat){
				return gmdate($dateFormat, (float) $v);
			}else{
				return (float) $v;
			}
		}else{
			return FALSE;
		}
	}

	/**
	 * Reads data in a Wordpress Meta Field.
	 *
	 * Data was saved using saveCustomFieldList.
	 *
	 * @see saveCustomFieldList
	 * @param int $post_id
	 * @param string $name
	 * @return multitype:
	 */
	public function readCustomFieldList($post_id, $name){
		$i = 1;
		$values=array();
		while( $value = get_post_meta($post_id, self::toKey($name . '_' . $i), true) ){
			array_push($values, $value);
			$i++;
		}
		return $values;
	}

	public function isPrivate($post_id){
		return in_array($this->getDatasetUUIDByPostId($post_id), $this->listPrivateDatasetUUIDs());
	}

	public function listDatasets(){
		$type = 'mksdc-datasets';
		$args=array(
				'post_type' => $type,
				'post_status' => 'publish',
				'ignore_sticky_posts'=> 1
		);
		$my_query = new WP_Query($args);
		$datasets = array();

		while ($my_query->have_posts()) {
			$my_query->the_post();
			$tags = get_the_tags();
			array_push($datasets, array(
				'permalink' => get_the_permalink(),
				'post_title' => get_the_title(),
				'content' => get_the_content(),
				'tags' => ($tags) ? $tags : array(),
				'license' => $this->getLicensesOfDatasetByPostId(get_the_id())
			));
		}
		wp_reset_query();
		return $datasets;
	}

	public function getLicensesOfDatasetByPostId($post_id){
		$license_ids = $this->readCustomFieldList($post_id, 'license');
		$licenses = array();
		foreach($license_ids as $lid){
			array_push($licenses, array(
				'id' => $lid,
				'title' => get_post_field('post_title', $lid)
			));
		}
		return $licenses;
	}

	public function getLicenseByPostId($post_id){
		$license = array();
		$license['post'] = $post = get_post($post_id);
		$license['taxonomies'] = wp_get_post_terms($post_id, array(
					// policies
					'mksdc-policy-scope',
					'mksdc-permissions',
					'mksdc-prohibitions',
					'mksdc-requirements'));
		$license['acf'] = get_post_meta($post_id);
		return $license;
	}

	public function saveDatasetCustomFields($post_id, $data){
		//print_r($_POST); die;
		MKSDC_Plugin::log("Saving Dataset Custom Fields: ". $post_id);
		$uuid = $this->readCustomField($post_id, 'uuid');
		MKSDC_Plugin::log(" - UUID is ". $uuid);
		if($uuid == FALSE){
			if($this->hasField('uuid', $data)) {
				// do nothing

			}else{
				// Generate one
				$data[self::toField('uuid')] = $this->gen_uuid();
				MKSDC_Plugin::log(" - Fixing dataset: creating UUID for ". $post_id);
			}
			MKSDC_Plugin::log(" - Saving uuid: ". $data['mksdc_uuid_field']);
			// save the custom field
			$this->saveCustomField($post_id, 'uuid', $data);
		}else{
			$data[self::toField('uuid')] = $uuid;
		}

		$this->saveCustomField($post_id, 'link_to_source', $data);
		$this->saveCustomField($post_id, 'link_to_source_desc', $data);
		$this->saveCustomField($post_id, 'attribution', $data);

		// Update the Policy info
		$this->saveCustomFieldList($post_id, 'license', $data);

		// Update the Services attached
		$this->saveCustomFieldArray($post_id, 'services', $data);

		// Update the Sources attached
		$oldSources = $this->readCustomFieldArray($post_id, 'sources');
		if(@$data[self::toField('sources')]){
			foreach ($data[self::toField('sources')] as $sourceIndex => &$source) {
				// A) Is it a file to be deleted?
				if(isset($source['delete']) && $source['delete'] == @$source['attachment_id'] ){
					// XXX Security check: was this object really attached to this form?
					$validAttachment = false;
					foreach($oldSources as $oldS){
						if(isset($oldS['attachment_id']) && $oldS['attachment_id'] == $source['attachment_id']){
							// It was ...
							$validAttachment = true;
						}
					}
					if($validAttachment){
						// XXX We just move to trash...
						if(FALSE === wp_delete_attachment($source['attachment_id'], FALSE)){
							MKSDC_Plugin::logErr('Cannot delete attachment ' . $source['attachment_id']);
						}else{
							// Succesfully delete. Let's remove the item from the array
							unset($data[self::toField('sources')][$sourceIndex]); 
						}
					}
				} else
				// B) If it is a previously uploaded file
				if(isset($source['attachment_id'])){
					// XXX Keep file meta data from existing value
					foreach($oldSources as $oldS){
						if(isset($oldS['attachment_id']) && $oldS['attachment_id'] == $source['attachment_id']){
							$source['file'] = $oldS['file'];
						}
					}
				}else
				// C) If it is a new file
				if(isset($source['file'])){
					// Upload the file and change the data
	
					$uploaded = wp_upload_bits( $source['file']['name'], null, @file_get_contents( $source['file']['tmp_name'] ) );
	
					if ( FALSE === $uploaded['error'] ) {
						MKSDC_Plugin::log('Uploaded file ' . $uploaded['file']);
						$filetype = wp_check_filetype( basename( $uploaded['file'] ), null );
						$upload_dir = wp_upload_dir();
						$attachment = array(
						        'guid' => $upload_dir['baseurl'] . _wp_relative_upload_path( $uploaded['file'] ),
						        'post_mime_type' => $filetype['type'],
						        'post_title' => preg_replace('/\.[^.]+$/', '', basename( $uploaded['file'] )),
						        'post_content' => '',
						        'post_status' => 'inherit'
						    );
						require_once(ABSPATH . 'wp-admin/includes/image.php');
						$attach_id = wp_insert_attachment( $attachment, $uploaded['file'], $post_id );
						$attach_data = wp_generate_attachment_metadata( $attach_id, $uploaded['file'] );
						wp_update_attachment_metadata( $attach_id, $attach_data );
						$source['attachment_id'] = $attach_id;
					}else{
						MKSDC_Plugin::logErr('Cannot upload file to wordpress' . $uploaded['error']);
					}
				}
			}
			// Update metadata
			$this->saveCustomFieldArray($post_id, 'sources', $data);
		}
		return $post_id;
	}

	public function createDataset($data = array()){
		# Can't have an ID if creating a new dataset, so make sure it's not there!
		unset($data['ID']);
		return $this->saveDataset($data);
	}

	public function updateDataset($post_id = null, $data = array()){
		if (!isSet($post_id)) {
			MKSDC_Plugin::logErr("Can't update a dataset without its ID!");
			return FALSE;
		}
		$data['ID'] = $post_id;
		return $this->saveDataset($data, true);
	}

	public function saveDataset($data, $updating = false){
		
		if ($updating) {
			$post_id = wp_update_post($data);
		} else {
			$post_id = wp_insert_post($data);
		}
		if ($post_id == 0 || $post_id instanceof WP_Error) {
			MKSDC_Plugin::logErr("Post creation failed! wp_insert_post returned: " . print_r($post_id, TRUE));
			return FALSE;
		}
		$this->saveDatasetCustomFields($post_id, $data);
		MKSDC_Plugin::log("Saved dataset with post_id: " . $post_id);
		return $post_id;
	}
	
	/**
	 * @param int $post_id
	 * @return array
	 */
	public function getDataset($post_id){
		$post = get_post($post_id);
		$data['post'] = $post;
		$data['content'] = $data['post']->post_content;
		// FIXME Remove the acf key (we don't want to expose the way data is stored...)
		$data['acf'] = $acf = get_post_meta($post_id);
		//
		$data['attribution'] = $this->readCustomField($post_id, 'attribution');
		$data['uuid'] = $acf['_mksdc_uuid_key'];
		$data['source'] = array('link' => $this->readCustomField($post_id, 'link_to_source'),
			'label' => $this->readCustomField($post_id, 'link_to_source_desc'));
		$data['owner'] = get_the_terms( $post , 'mksdc-dataowners', 'objects');
		$data['formats'] = get_the_terms( $post , 'mksdc-dataformats', 'objects');
		//$data['status'] = get_the_terms( $post , 'mksdc-datastatus');

		//
		$data['categories'] = get_the_category($post_id);
		$data['tags'] = get_the_tags($post_id);
		$data['license'] = $this->getLicensesOfDatasetByPostId($post_id);
		$data['isPrivate'] = false;// hardwired for now (this is legacy) $this->isPrivate($post_id);
		$data['services'] = $this->getServices($post_id);
		$data['sources'] = $this->getSources($post_id);
		$mksdc = MKSDC_Plugin::instance();
		$extensions = $mksdc->getExtensions('dataset');

		foreach($extensions as $key => $ext){
			// We don't override pre-existing data!
			if(isset($data[$key])){
				continue;
			}
			if(is_array($ext)){
				$obj = $ext[0];
				$met = $ext[1];
				$data[$key] = $obj->$met($post_id);
			}elseif(function_exists($ext)){
				$data[$key] = $ext($post_id);
			}
			#echo "<!--\n$key\n";print_r($data[$key]);echo "-->\n";
		}

		return $data;
	}

	/**
	 * Get the WP post of a dataset by UUID
	 *
	 * @param string $uuid
	 * @return object|boolean
	 */
	public function getDatasetByUUID($uuid){
		$args = array(
			'meta_key' => self::toKey('uuid'),
			'meta_value' => $uuid,
			'post_type' => 'mksdc-datasets',
			'posts_per_page' => 1
		);
		$post = get_posts($args);
		if(count($post) == 1){
			return $post[0];
		}
		return FALSE;
	}

	public function getDatasetUUIDByPostId($post_id){
		return $this->readCustomField($post_id, 'uuid');
	}

	public function listPrivateDatasetUUIDs(){
		// Private datasets contributed by extensions
		$plugin = MKSDC_Plugin::instance();
		$extensions = $plugin->getExtensions('private');
		$private = array();
		foreach($extensions as $key => $ext){
			$srvs = array();
			if(is_array($ext)){
				$obj = $ext[0];
				$met = $ext[1];
				$srvs = $obj->$met();
			}elseif(function_exists($ext)){
				$srvs = $ext();
			}
			foreach($srvs as $srv){
				array_push($private, $srv);
			}
		}
		//
		return $private;
	}

	public function getSources($post_id){
		$sources = array();
		// Sources
		$others = $this->readCustomFieldArray($post_id, 'sources');
		if(is_array($others)){
			foreach($others as $o){
				array_push($sources, $o);
			}
		}
		return $sources;
	}
	
	public function getServices($post_id){
		$services = array();
		// Services configured from the admin interface
		$others = $this->readCustomFieldArray($post_id, 'services');
		if(is_array($others)){
			foreach($others as $o){
				array_push($services, $o);
			}
		}

		// Services contributed by extensions
		$plugin = MKSDC_Plugin::instance();
		$extensions = $plugin->getExtensions('service');

		foreach($extensions as $key => $ext){
			$srvs = array();
			if(is_array($ext)){
				$obj = $ext[0];
				$met = $ext[1];
				$srvs = $obj->$met($post_id);
			}elseif(function_exists($ext)){
				$srvs = $ext($post_id);
			}
			foreach($srvs as $srv){
				array_push($services, $srv);
			}
		}
		return $services;
	}
	/**
	 * Taken from
	 * http://php.net/manual/en/function.uniqid.php#94959
	 *
	 * @return string
	 */
	function gen_uuid() {
		return sprintf( '%04x%04x-%04x-%04x-%04x-%04x%04x%04x',
				// 32 bits for "time_low"
				mt_rand( 0, 0xffff ), mt_rand( 0, 0xffff ),

				// 16 bits for "time_mid"
				mt_rand( 0, 0xffff ),

				// 16 bits for "time_hi_and_version",
				// four most significant bits holds version number 4
				mt_rand( 0, 0x0fff ) | 0x4000,

				// 16 bits, 8 bits for "clk_seq_hi_res",
				// 8 bits for "clk_seq_low",
				// two most significant bits holds zero and one for variant DCE1.1
				mt_rand( 0, 0x3fff ) | 0x8000,

				// 48 bits for "node"
				mt_rand( 0, 0xffff ), mt_rand( 0, 0xffff ), mt_rand( 0, 0xffff )
		);
	}

	public static function instance(){
		if(self::$_instance === NULL){
			self::$_instance = new MKSDC_DataAccess();
		}else{
		}
		return self::$_instance;
	}

	public static function isAlive(){
		return (self::$_instance !== NULL);
	}

	public static function shutdown(){
		self::$_instance = NULL;
	}
}

function mksdc_sorter_policy_options($a, $b){
	if($a == 'Other'){
		return 1;
	}else if($b == 'Other'){
		return 0;
	}
	if ($a == $b) {
		return 0;
	}
	return ($a < $b) ? -1 : 1;
}

function mksdc_da_shutdown(){
	MKSDC_DataAccess::shutdown();
}
// Close connection before exit
register_shutdown_function('mksdc_da_shutdown');
