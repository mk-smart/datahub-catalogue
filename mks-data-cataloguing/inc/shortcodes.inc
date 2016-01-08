<?php 
/**
 * Creates a link to a page from the template name, appends all other parameters as query params
 */
function mksdc_shortcode_datacatalogue($atts){
	return MKSDC_Plugin::getPageFromTemplate(MKSDC_Plugin::DATA_CATALOGUE_TMPL, $atts);	
}
add_shortcode('datacatalogue', 'mksdc_shortcode_datacatalogue');

function mksdc_shortcode_dataset($atts){
	return MKSDC_Plugin::getPageFromTemplate(MKSDC_Plugin::DATA_CATALOGUE_TMPL, $atts);
}


function mksdc_shortcode_count_datasets($atts){
	$args = $atts;
	$args['post_type'] = 'mksdc-datasets';
	$args['fields'] = 'ids';
	$args['posts_per_page'] = -1;
	$args['no_found_rows'] = true;
	
	$search = new WP_Query($args);
	$search->have_posts();
// 	print '<pre>';
// 	print_r($search);
// 	print '</pre>';
	
	return $search->post_count;
}

add_shortcode('dataset', 'mksdc_shortcode_dataset');

add_shortcode('count-datasets', 'mksdc_shortcode_count_datasets');


