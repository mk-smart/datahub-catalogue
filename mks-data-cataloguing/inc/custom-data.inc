<?php
// Policy meta box
function mksdc_inner_policy_box( $post ) {
	// Add an nonce field so we can check for it later.
	wp_nonce_field( 'mksdc_inner_custom_box', 'mksdc_inner_custom_box_nonce' );
	$licenses = array();
	$i = 1;
	while(true){
		$lic=get_post_meta($post->ID, '_mksdc_license_'.$i.'_key', true);
		if(!$lic) break;
		array_push($licenses, $lic);
		$i++;
	}
	$html = new MVC_Html(array('licences' => $licenses));
	$html->render('field-licence');
}

function mksdc_inner_sources_box($post){
	$d = MKSDC_DataAccess::instance();
	$sources = $d->readCustomFieldArray($post->ID, 'sources');
	echo '<div>';
	echo '<div id="mksdc_sources_list"><ul>';
	if(is_array($sources)){
		foreach($sources as $ss){
			print '<li>';
			print '<a href="';
			if(isset($ss['attachment_id'])){
				// show attachment
				print @wp_get_attachment_url(@$ss['attachment_id']);
			}else{
				// link to source
				print $ss['link'];
			}
			print '">' . $ss['title'] . '</a>';
			// if it is file
			if(isset($ss['file'])){
				print ' (' . $ss['file']['type'] . ')';
			}
			print '</li>';
		}
	}
	echo '</ul></div>';
	echo '</div>';
}
function mksdc_inner_services_box($post){
	$d = MKSDC_DataAccess::instance();
	$services = $d->readCustomFieldArray($post->ID, 'services');
	echo '<div>';
	echo '<div id="mksdc_services_list">';
	// Existing services
	if(is_array($services))
	foreach($services as $six => $service){
		$title = @$service['title'];
		$url = @$service['url'];
		echo <<<BOX
		<div class="mksdc_service_box" id="mksdc_service_box_$six" data-index="$six">
				<strong>Service</strong>
				<input style="width: 100%" name="mksdc_services_field[$six][title]" value="$title" placeholder="name of the service" type="text">
				<strong>URL</strong>
				<input style="width: 100%" name="mksdc_services_field[$six][url]" value="$url" placeholder="URL of the service" type="text">		
BOX;
		echo 'Buttons:';
		echo '<div class="mksdc_service_actions_list" id="mksdc_service_'.$six.'_action_list" data-index="'.$six.'">';
		// Add buttons
		foreach(@$service['actions'] as $aix => $action){
			$label = $action['label'];
			$link = $action['link'];
			$boxid = 'mksdc_service_' . $six . '_action_' . $aix;
			echo <<<BOX
				<div id="$boxid" class="mksdc_service_action" data-index="$aix">
				<select class="mksdc_service_action_type" name="mksdc_services_field[$six][actions][$aix][type]">
BOX;
			$types = array('primary', 'success', 'warning', 'danger', 'info');
			foreach($types as $type){
				$selected = ($type == $action['type']) ? 'SELECTED' : '';
				echo "<option value=\"$type\" $selected>$type</option>"; 
			}
			echo <<<BOX
				</select>
				<input class="mksdc_service_action_label" name="mksdc_services_field[$six][actions][$aix][label]" value="$label" placeholder="label" type="text">
				<input class="mksdc_service_action_link" style="width: 40%" name="mksdc_services_field[$six][actions][$aix][link]" value="$link" placeholder="link" type="text">&nbsp;<a class="mksdc_action_link" onclick="MKSDC.removeServiceAction(this)" data-target="$boxid">remove</a>
				</div>
BOX;
		}
		echo '</div>'; // close actions
		echo <<<BOX
			<div><a class="mksdc_action_link" id="add_another_service_action" onclick="MKSDC.addServiceAction($six)">add button</a>&nbsp|&nbsp;<a class="mksdc_action_link" onclick="MKSDC.removeService(this)" data-target="mksdc_service_box_$six">remove service</a></div>
					
					
BOX;
		echo '</div>'; // close service
	}
	// close services list
	echo '</div>';
	echo '';
	echo '<div><a class="mksdc_action_link" id="add_another_service" onclick="MKSDC.addService()">add a service</a></div>';
	echo '</div>';
	echo <<<SCRIPT
  <script>
if(typeof MKSDC == 'undefined'){
	MKSDC = {};
}
MKSDC.addServiceAction = function(index){
	var $ = jQuery;
	var actionList = $('#mksdc_service_'+index+'_action_list');
	var aix = parseInt(actionList.find('.mksdc_service_action').last().data('index')) + 1;
	//$('#mksdc_service_'+index+'_action_list div.mksdc_service_action:last a.mksdc_action_link').hide();
	actionList.append(MKSDC.buildActionBox(index,aix));
}

MKSDC.buildActionBoxSelectType = function(six,aix,type){
	var $ = jQuery;
	var box = $('<select class="mksdc_service_action_type" name="mksdc_services_field['+six+'][actions]['+aix+'][type]"></select>');
	box.append('<option value="primary">primary</option>');
	box.append('<option value="success">success</option>');
	box.append('<option value="warning">warning</option>');
	box.append('<option value="danger">danger</option>');
	box.append('<option value="info">info</option>');
	return box;
}
MKSDC.buildActionBox = function(six, aix){
	var $ = jQuery;
	var box = $('<div id="mksdc_service_'+six+'_action_'+aix+'" class="mksdc_service_action" data-index="'+aix+'"></div>');
	box.append(MKSDC.buildActionBoxSelectType(six,aix));
	box.append('<input class="mksdc_service_action_label" type="text" name="mksdc_services_field['+six+'][actions]['+aix+'][label]" value="" placeholder="label"/>');
	box.append('<input class="mksdc_service_action_link"  style="width: 40%" type="text" name="mksdc_services_field['+six+'][actions]['+aix+'][link]" value="" placeholder="link"/>');
	box.append('&nbsp;<a class="mksdc_action_link" onClick="MKSDC.removeServiceAction(this)" data-target="mksdc_service_'+six+'_action_'+aix+'">remove</a>');
	return box;	
}
MKSDC.buildServiceBox = function(index){
	var $ = jQuery;
	var box = $('<div class="mksdc_service_box" id="mksdc_service_box_'+index+'" data-index="'+index+'"></div>');
	box.append('<strong>Service</strong>');
	box.append('<input style="width: 100%" type="text" name="mksdc_services_field['+index+'][title]" value="" placeholder="name of the service"/>');
	box.append('<strong>URL</strong>');
	box.append('<input style="width: 100%" type="text" name="mksdc_services_field['+index+'][url]" value="" placeholder="URL of the service"/>');
	box.append('Buttons:');
	var alBox = $('<div class="mksdc_service_actions_list" id="mksdc_service_'+index+'_action_list" data-index="'+index+'"></div>');
	alBox.append(MKSDC.buildActionBox(index,0));
	box.append(alBox);
	box.append('<div><a class="mksdc_action_link" id="add_another_service_action" onclick="MKSDC.addServiceAction('+index+')">add button</a>&nbsp;|&nbsp;<a class="mksdc_action_link" onClick="MKSDC.removeService(this)" data-target="mksdc_service_box_'+index+'">remove service</a></div>');
	return box;
}
MKSDC.addService = function (){
	var $ = jQuery;
	var six = parseInt($('#mksdc_services_list').find('.mksdc_service_box').last().data('index')) + 1;
	if(isNaN(six)){
		six=0;
	}
	var serviceBox = MKSDC.buildServiceBox(six);
  	// add item
  	$('#mksdc_services_list').append(serviceBox);
}
MKSDC.removeService = function (a){
	var $ = jQuery;
	var trgt = $(a).data('target');
  	$('#'+trgt).detach();
}
MKSDC.removeServiceAction = function (a){
	var $ = jQuery;
	var trgt = $(a).data('target');
	$('#'+trgt).prev().find('a.mksdc_action_link').show();
  	$('#'+trgt).detach();
}
  </script>
SCRIPT;
}

function mksdc_custom_field_simpletext($post, $fieldId, $options = array()){
	// Add an nonce field so we can check for it later.
	wp_nonce_field( 'mksdc_inner_custom_box', 'mksdc_inner_custom_box_nonce' );
	$fieldKey = MKSDC_DataAccess::toKey($fieldId);
	$field = MKSDC_DataAccess::toField($fieldId);
	$value = get_post_meta($post->ID, $fieldKey, true);
	if(isset($options['type']) && $options['type'] == 'textarea'){
		echo '<textarea style="width:80%" rows="5" name="' . $field . '">' . $value . '</textarea>';
	}else{
		echo '<input type="text" style="width:80%" name="' . $field . '" value="' . $value . '"/>';
	}
}

function mksdc_inner_uuid_box($post){
	$d = MKSDC_DataAccess::instance();
	// Read Only
	$uuid = $d->readCustomField($post->ID, 'uuid');
	
	print '<div>';
	$table = array(
		'rows' => array(
			array(array('th', 'UUID'), $uuid),
		)
	);
	$v = new MVC_Viewer($table);
	$v->render('table-simple');
	print '</div>';
}

function mksdc_inner_rdfcat_box($post){
	$d = MKSDC_DataAccess::instance();
	// Read Only
	$uuid = $d->readCustomField($post->ID, 'uuid');

	print '<div>';
	$table = array(
			'rows' => array(
					array(array('th', 'Created on RDF Cat'), $d->readCustomFieldTimestamp($post->ID, 'createdonrdfcat', "Y-m-d\TH:i:s\Z")),
					array(array('th', 'Updated on RDF Cat'), $d->readCustomFieldTimestamp($post->ID, 'updatedonrdfcat', "Y-m-d\TH:i:s\Z")),
			)
	);
	$v = new MVC_Viewer($table);
	$v->render('table-simple');
	print '</div>';
}

function mksdc_inner_attribution_box($post){
	mksdc_custom_field_simpletext($post, 'attribution', array('type'=>'textarea'));
}

function mksdc_inner_link_to_source_box($post){
	print '<table class="mksdc" style="width: 100%">';
	print '<tr>';
	print "<th>Name of source:</th><td>";
    mksdc_custom_field_simpletext($post, 'link_to_source_desc');
    print '</td></tr>';
	print "<tr><th>Link:</th><td>";
	mksdc_custom_field_simpletext($post, 'link_to_source');
	print '</td></tr>';
	print '</table>';
}

// Add meta boxes
function mksdc_add_meta_boxes() {
	add_meta_box('mksdc_uuid_box', __( 'Identification', 'mksdc_textdomain' ), 'mksdc_inner_uuid_box', 'mksdc-datasets');
	add_meta_box('mksdc_link_to_source_box', __( 'Link to source', 'mksdc_textdomain' ), 'mksdc_inner_link_to_source_box', 'mksdc-datasets');
	add_meta_box('mksdc_attribution_box', __( 'Attribution statement', 'mksdc_textdomain' ), 'mksdc_inner_attribution_box', 'mksdc-datasets');
	add_meta_box('mksdc_policy_box',__( 'Redistribution policy / license', 'mksdc_textdomain' ),'mksdc_inner_policy_box','mksdc-datasets');
	add_meta_box('mksdc_sources_box',__( 'Sources', 'mksdc_textdomain' ),'mksdc_inner_sources_box','mksdc-datasets');
	add_meta_box('mksdc_services_box',__( 'Services', 'mksdc_textdomain' ),'mksdc_inner_services_box','mksdc-datasets');
	add_meta_box('mksdc_rdfcat_box', __( 'RDF Catalogue mirroring', 'mksdc_textdomain' ), 'mksdc_inner_rdfcat_box', 'mksdc-datasets');
}
add_action( 'add_meta_boxes', 'mksdc_add_meta_boxes' );

function mksdc_save_postdata( $post_id ) {
	
	// Check if our nonce is set.
	if ( ! isset( $_POST['mksdc_inner_custom_box_nonce'] ) )
		return $post_id;

	$nonce = $_POST['mksdc_inner_custom_box_nonce'];

	// Verify that the nonce is valid.
	if ( ! wp_verify_nonce( $nonce, 'mksdc_inner_custom_box' ) )
		return $post_id;

	// If this is an autosave, our form has not been submitted, so we don't want to do anything.
	// XXX ...but it is an auto"save"... why we don't save our changes as draft (investigate)?
	if ( defined( 'DOING_AUTOSAVE' ) && DOING_AUTOSAVE )
		return $post_id;

	// Check the user's permissions.
	if ( 'page' == $_POST['post_type'] ) {
		if ( ! current_user_can( 'edit_page', $post_id ) )
			return $post_id;
	} else {

		if ( ! current_user_can( 'edit_post', $post_id ) )
			return $post_id;
	}

	require_once('dataaccess.inc');
	/* OK, its safe for us to save the custom fields now. */
	$dataaccess = MKSDC_DataAccess::instance();
	$dataaccess->saveDatasetCustomFields($_POST['ID'], $_POST);
}
add_action( 'save_post', 'mksdc_save_postdata' );





// don't close <?php