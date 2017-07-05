<?php
$mksdc_pt = array(
		'labels' => array(
				'name' => __( 'Datasets' ),
				'singular_name' => __( 'Dataset' )
		),
		'public' => true,
		'has_archive' => false,
		'description' => 'Datasets catalogued with metadata',
		// should be lighter (and link between bubbles thicker)
		// also should not be an image
		'menu_icon' => plugins_url().'/mks-data-cataloguing/img/data-grey-icon-16.png',
		// Use the following taxonomies with this post type
		'taxonomies' => array('category',
				'post_tag',
				'mksdc-dataformats',
				'mksdc-datastatus',
				'mksdc-dataowners'
		),
		'rewrite' => array( 'slug' => 'dataset' ),
		'supports' => array('title',
				'editor',
				//'comments',
				'revisions'
		)
);
register_post_type( 'mksdc-datasets', $mksdc_pt);
unset($mksdc_pt);

$mksdc_pt = array(
		'labels' => array(
				'name' => __( 'Redistribution policies' ),
				'singular_name' => __( 'Redistribution policy' )
		),
		'public' => true,
		'has_archive' => true,
		'description' => 'Licenses and policies related to the redistribution of datasets',
		//'menu_icon' => plugins_url().'/mks-data-cataloguing/img/data-grey-icon-16.png',
		'taxonomies' => array('category',
			'mksdc-policy-scope',
			'mksdc-permissions',
			'mksdc-prohibitions',
			'mksdc-requirements'
		),
		'rewrite' => array( 'slug' => 'policy' ),
		'supports' => array('title',
				'editor',
				//'comments',
				'revisions',
		),
);
register_post_type( 'mksdc-policies', $mksdc_pt);

/**
 * To close comments on post type mksdc-datasets and mksdc-policies
 *
 * @param array $posts
 * @return array
 */
function mksdc_close_comments( $posts ) {
	if ( !is_single() ) { return $posts; }
	if($posts && is_array($posts)){
		$post_type = get_post_type($posts[0]->ID) ;
		if ( 'mksdc-datasets' == $post_type || 'mksdc-policies' == $post_type) {
			$posts[0]->comment_status = 'closed';
			$posts[0]->ping_status    = 'closed';
			wp_update_post( $posts[0] );
		}
	}
	return $posts;
}
add_filter( 'the_posts', 'mksdc_close_comments' );

function mksdc_get_custom_post_type_template($single_template) {
	global $post;
	if ($post->post_type == 'mksdc-datasets') {
		$plugin = MKSDC_Plugin::instance();
		$single_template = $plugin->getDatasetPageTemplate($post->ID);
	}
	return $single_template;
}
add_filter( 'single_template', 'mksdc_get_custom_post_type_template' );
