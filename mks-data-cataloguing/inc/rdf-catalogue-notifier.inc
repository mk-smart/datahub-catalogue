<?php

// saving function
function mksdc_notify_rdf_catalogue($post_id, $type, $action){
	$db = MKSDC_DataAccess::instance();
	$p = get_post($post_id);
	if($type == 'mksdc-datasets'){
		$jsonld = MKSDC_JSONLD::dataset($p->post_name);
		$uuid = $jsonld->{"@id"};
		$graph = MKSDC_Plugin::getLinkedDataURI('dataset/' . $uuid);
	}else if($type == 'mksdc-policies'){
		$jsonld = MKSDC_JSONLD::policy($p->post_name);
		$p = get_post($post_id);
		$graph = MKSDC_Plugin::getLinkedDataURI('policy/' . $p->post_name);
	}else{
		return;
	}

	$url = MKSDC_Plugin::getRDFCatUrl();
	$key = MKSDC_Plugin::getRDFCatApiKey();
	$jsonld = json_encode($jsonld);
	
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL,  $url );
	curl_setopt($ch, CURLOPT_POST, 1);
	curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-type: application/x-www-form-urlencoded; charset=utf8' . $key, 'another: bob' ));
	curl_setopt($ch, CURLOPT_USERPWD, "$key:");
	curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query(array('action'=>$action, 'graph'=>$graph, 'json'=>base64_encode($jsonld), 'timestamp'=>time())));
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	$server_output = curl_exec ($ch);
	$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
	curl_close ($ch);
	if(strpos($httpCode, '2') !== 0){
		MKSDC_Plugin::logErr("RDF Cat manager notification FAILED. HTTP Response was: " . $httpCode . ", " . $server_output);
		return FALSE;
	}else{
		MKSDC_Plugin::log("RDF Cat manager notification: post=$post_id, action=$action . OK");
	}
	
	if($db->readCustomField($post_id, 'createdonrdfcat')){
		$db->saveMetaField($post_id, 'updatedonrdfcat', time());
	}else{
		$db->saveMetaField($post_id, 'createdonrdfcat', time());
	}
	return $server_output;
}

function mksdc_observer( $post_id ){
	
	$react = FALSE;
	$logas = '';
	// If it is a bulk update
	if(isset($_GET['bulk_edit']) && $_GET['bulk_edit'] == 'Update' & isset($_GET['_wpnonce'])){
		$react = TRUE;
		$logas = 'bulk update';
	}else
	// Check if it is a post with a nonce.
	if ( isset( $_POST['action'] ) && isset($_POST['_wpnonce']) ){
		$react = TRUE;
		$logas = 'update';
	}
	
	// If this is an autosave, our form has not been submitted, so we don't want to do anything.
	if ( defined( 'DOING_AUTOSAVE' ) && DOING_AUTOSAVE ){
		$react = FALSE;
	}
	
	if(!$react) {
		return;
	}
	
	MKSDC_Plugin::log('RDF Cat manager notification: about ' . $post_id . ' during ' . $logas);
	
	// We only handle some post types
	$status = get_post_status($post_id);
	$ptype = get_post_type($post_id);
	if($ptype === false) return; // do nothing
	
	$types = array('mksdc-datasets','mksdc-policies');
	// Map between
	$actions = array(
			'new' => 'put',
			'trash' => 'delete',
			'publish' => 'put',
			'draft' => 'put',
			'pending' => 'put',
			'private' => 'put',
			// 'auto-draft' => 'ignore',
			// 'future' => 'ignore',
			// 'inherit' => 'ignore',
	);
	if((!in_array($ptype, $types)) || (!array_key_exists($status, $actions))){
		// do nothing
		return;
	}
	
	
	// Check the user's permissions.
	if ( 'page' == $ptype ) {
		if ( ! current_user_can( 'edit_page', $post_id ) )
			return $post_id;
	} else {
		if ( ! current_user_can( 'edit_post', $post_id ) )
			return $post_id;
	}

	MKSDC_Plugin::log('Mirroring post: ' . $post_id . ' (' . $ptype . ')');
	// Notify the RDF catalogue updater
	mksdc_notify_rdf_catalogue($post_id, $ptype, $actions[$status]);
}
add_action('save_post', 'mksdc_observer');

// // This is to add custom fields to the json output
// function json_api_encode_acf($response) {
// 	if (isset($response['posts'])) {
// 		foreach ($response['posts'] as $post) {
// 			json_api_add_acf($post); // Add specs to each post
// 		}
// 	}
// 	else if (isset($response['post'])) {
// 		json_api_add_acf($response['post']); // Add a specs property
// 	}
// 	return $response;
// }
// add_filter('json_api_encode', 'json_api_encode_acf');

// function json_api_add_acf(&$post){
// 	$post->acf = get_post_meta($post->id); 
// 	$db = MKSDC_DataAccess::instance();
// 	$data = $db->getDataset($post->id);
// 	foreach($data as $k => $v){
// 		if(!isset($post->$k)){
// 			$post->$k = $v;
// 		}
// 	}
// }
