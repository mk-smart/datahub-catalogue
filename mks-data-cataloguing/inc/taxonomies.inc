<?php
$mkst_dataformats = array(
		'labels' => array(
				'name' => __( 'Data formats' ),
				'singular_name' => __( 'Data format' )
		),
		'public' => true,
		'hierarchical' => true,
		'show_ui'               => true,
		'show_admin_column'     => true,
		'update_count_callback' => '_update_post_term_count',
		'query_var'             => true,
		'rewrite'               => array( 'slug' => 'data-formats' ),
		// rewrite
);
register_taxonomy('mksdc-dataformats', array ( 'mksdc_datasets' ), $mkst_dataformats);
unset($mkst_dataformats);

$mkst_dataowners = array(
		'labels' => array(
				'name' => __( 'Data owners' ),
				'singular_name' => __( 'Data owner' )
		),
		'public' => true,
		//			  'hierarchical' => true,
		'show_ui'               => true,
		'show_admin_column'     => true,
		'update_count_callback' => '_update_post_term_count',
		'query_var'             => true,
		'rewrite'               => array( 'slug' => 'data-owner' ),
);
register_taxonomy('mksdc-dataowners', array ( 'mksdc_dataowners' ), $mkst_dataowners);
unset($mkst_dataowners);

$mkst_datastatus = array(
		'labels' => array(
				'name' => __( 'Data status' ),
				'singular_name' => __( 'Data status' )
		),
		'public' => true,
		//			  'hierarchical' => true,
		'show_ui'               => true,
		'show_admin_column'     => true,
		'update_count_callback' => '_update_post_term_count',
		'query_var'             => true,
		'rewrite'               => array( 'slug' => 'data-status' ),
);
register_taxonomy('mksdc-datastatus', array ( 'mksdc_datastatus' ), $mkst_datastatus);
unset($mkst_datastatus);

$mkst_scope = array(
		'labels' => array(
				'name' => __( 'Scopes' ),
				'singular_name' => __( 'Scopes' )
		),
		'public' => true,
		'hierarchical' => true,
		'show_ui'               => true,
		'show_admin_column'     => true,
		'update_count_callback' => '_update_post_term_count',
		'query_var'             => true,
		'rewrite'               => array( 'slug' => 'data-policy-scope' )
)
;
register_taxonomy('mksdc-policy-scope', array ( 'mksdc_policy_scope' ), $mkst_scope);
unset($mkst_scope);

$mkst_permissions = array(
		'labels' => array(
				'name' => __( 'Permissions' ),
				'singular_name' => __( 'Permission' )
		),
		'public' => true,
		'hierarchical' => true,
		'show_ui'               => true,
		'show_admin_column'     => true,
		'update_count_callback' => '_update_post_term_count',
		'query_var'             => true,
		'rewrite'               => array( 'slug' => 'data-permissions' )
);
register_taxonomy('mksdc-permissions', array ( 'mksdc_permissions' ), $mkst_permissions);
unset($mkst_permissions);

$mkst_prohibitions = array(
		'labels' => array(
				'name' => __( 'Prohibitions' ),
				'singular_name' => __( 'Prohibition' )
		),
		'public' => true,
		'hierarchical' => true,
		'show_ui'               => true,
		'show_admin_column'     => true,
		'update_count_callback' => '_update_post_term_count',
		'query_var'             => true,
		'rewrite'               => array( 'slug' => 'data-prohibitions' ),
		// rewrite
);
register_taxonomy('mksdc-prohibitions', array ( 'mksdc_prohibitions' ), $mkst_prohibitions);
unset($mkst_prohibitions);

$mkst_requirements = array(
		'labels' => array(
				'name' => __( 'Requirements' ),
				'singular_name' => __( 'Requirement' )
		),
		'public' => true,
		'hierarchical' => true,
		'show_ui'               => true,
		'show_admin_column'     => true,
		'update_count_callback' => '_update_post_term_count',
		'query_var'             => true,
		'rewrite'               => array( 'slug' => 'data-requirements' ),
		// rewrite
);
register_taxonomy('mksdc-requirements', array ( 'mksdc_requirements' ), $mkst_requirements);
unset($mkst_requirements);

/* register_taxonomy('mksdc-datastorage', array ( 'mksdc_datastorage' ), */
/* 		      array( */
/* 		      'labels' => array( */
/*                             'name' => __( 'Storage destinations' ), */
/*                             'singular_name' => __( 'Storage destination' ) */
/*                         ), */
/* 			  'public'		  => true, */
/* 			  'hierarchical'	  => true, */
/* 			  'show_ui'               => true, */
/* 			  'show_admin_column'     => true, */
/* 			  'update_count_callback' => '_update_post_term_count', */
/* 			  'query_var'             => true, */
/* 			  'rewrite'               => array( 'slug' => 'data-storage' ), */
/* 			  // rewrite */
/* 		      ) */
/* 		     ); */