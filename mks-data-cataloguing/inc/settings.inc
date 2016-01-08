<?php
require_once(dirname(__FILE__) . '/mvctools.inc');

add_action('admin_menu', 'mksdc_plugin_settings');
function mksdc_plugin_settings() {
	// 	add_menu_page('Data Catalogue', 'Data Catalogue', 'administrator', 'mksdc_admin', 'mksdc_display_settings');
	// 	add_submenu_page('mksdc_admin', 'Settings', 'Settings', 'administrator', 'mksdc_settings', 'mksdc_display_settings');
	add_menu_page('Data Catalogue', 'Data Catalogue', 'administrator', 'mksdc-settings', 'mksdc_display_settings');
}

function mksdc_display_settings(){
	$v = new MVC_Html();
	$v->render('settings-page');
}

/**
 * Initializes the theme options page by registering the Sections,
 * Fields, and Settings.
 *
 * This function is registered with the 'admin_init' hook.
 */
add_action('admin_init', 'mksdc_init_settings');
function mksdc_init_settings() {
	// RDFCAT API
	add_settings_section('mksdc_rdfcat_section', 'RDF Catalogue Mirror', 'mksdc_rdfcat_section_callback', 'mksdc-settings');
	add_settings_field('mksdc_rdfcat_url', 'URL', 'mksdc_rdfcat_url_callback', 'mksdc-settings', 'mksdc_rdfcat_section');
	add_settings_field('mksdc_rdfcat_key', 'Auth Key', 'mksdc_rdfcat_key_callback', 'mksdc-settings', 'mksdc_rdfcat_section');
	register_setting( 'mksdc-settings', 'mksdc_rdfcat_url', 'mksdc_val_text' );
	register_setting( 'mksdc-settings', 'mksdc_rdfcat_key', 'mksdc_val_text' );
	
}
function mksdc_rdfcat_section_callback(){
	echo "<p>Configure details of connection and credentials to the RDF catalogue mirror.</p>";
}

// Shared, prints a text input
function mksdc_settings_field_text($field, $default = '', $description = ''){
	$v = new MVC_Html(array(
			'name' => $field,
			'id' => $field,
			'type' => 'text',
			'class' => 'regular-text code',
			'value' => get_option( $field, $default ),
			'description' => $description
	));
	$v->render('field-text');
}

// Shared, prints a textarea input
function mksdc_settings_field_textarea($field, $default = '', $description = ''){
	$v = new MVC_Html(array(
			'name' => $field,
			'id' => $field,
			'type' => 'text',
			'class' => 'textarea code',
			'style' => 'width: 90%',
			'value' => get_option( $field, $default ),
			'description' => $description
	));
	$v->render('field-textarea');
}


// RDF Mirror
function mksdc_rdfcat_url_callback(){
	mksdc_settings_field_text('mksdc_rdfcat_url','','eg: http://www.example.org/api');
}
function mksdc_rdfcat_key_callback(){
	mksdc_settings_field_text('mksdc_rdfcat_key','','eg: http://www.example.org/api');
}

// Sanitatizing functions
function mksdc_val_text($v){
	return trim($v);
}
function mksdc_val_int($v){
	return intval($v);
}

