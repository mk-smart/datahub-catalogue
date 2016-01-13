<?php

class MKSDC_JSONLD{

	private static function _context(){
		$url = MKSDC_Plugin::getLinkedDataURI();
		$CTX=<<<END
{
	"dcat": "http://www.w3.org/ns/dcat#",
	"dct":	"http://purl.org/dc/terms/",
	"dctype":	"http://purl.org/dc/dcmitype/",
	"foaf":	"http://xmlns.com/foaf/0.1/",
	"rdf":	"http://www.w3.org/1999/02/22-rdf-syntax-ns#",
	"rdfs":	"http://www.w3.org/2000/01/rdf-schema#",
	"skos":	"http://www.w3.org/2004/02/skos/core#",
	"vcard":	"http://www.w3.org/2006/vcard/ns#",
	"xsd":	"http://www.w3.org/2001/XMLSchema#",
	"records": "http://www.w3.org/ns/dcat#record",
	"odrl": "http://www.w3.org/ns/odrl/2/",
	"ns": "{{url}}",
	"record": "{{url}}record/",
	"dataset": "{{url}}dataset/",
	"services": "http://www.w3.org/ns/dcat#distribution",
	"distribution": "{{url}}distribution/",
	"policies": "cat:policy",
	"policy": "{{url}}policy/",
	"act": "{{url}}action/",
	"category": "{{url}}category/",
	"uuid": "{{url}}schema/uuid",
	"action" : {"@id" : "odrl:action", "@type" : "@id"},
	"cat": "{{url}}schema/",
	"name": "{{url}}schema/name",
	"attribution": "{{url}}schema/attribution",
	"api": {
		"@id" : "{{url}}schema/api", "@type" : "xsd:anyURI"
	},
	"primaryTopic":	"foaf:primaryTopic",
	"homepage":	"foaf:homepage",
	"title":	"dct:title",
	"issued":	"dct:issued",
	"modified":	"dct:modified",
	"description":	"dct:description",
	"landingPage": {"@id":"dcat:landingPage", "@type": "@id"},
	"theme":{"@id" : "cat:theme", "@type": "@id"},
	"permissions": {"@id" : "odrl:permission","@type": "@id"},
	"prohibitions": {"@id" : "odrl:prohibition","@type": "@id"},
	"duty": {"@id" : "odrl:duty", "@type": "@id"}
  }
	
END;
	
		return json_decode(trim(str_replace('{{url}}',$url,$CTX)));
	}
	
	public static function catalogue(){
		$db = MKSDC_DataAccess::instance();
		$args=array(
				'post_type' => 'mksdc-datasets',
				'post_status' => 'publish',
				'ignore_sticky_posts'=> 1,
				'posts_per_page' => -1,
		);
		$privateUUIDs = $db->listPrivateDatasetUUIDs();
		$args['meta_query'] = array(
				array(
					'key' => MKSDC_DataAccess::toKey('uuid'),
					'value' => $privateUUIDs,
					'compare' => 'NOT IN'
				)
		);
		
		$my_query = new WP_Query($args);
		$o = new stdClass();
		$ctx = "@context";
		$o->$ctx = self::_context();
		$o->{"title"} = "Data Catalogue";
		$o->{"@type"} = "dcat:Catalog";
		$o->{"@id"} = "ns:dcat";
		$o->records = array();
		while ($my_query->have_posts()) {
			$my_query->the_post();
			array_push($o->records, self::_record(get_the_ID()));
		}
		return $o;
	}
	
	private static function _record($post_id){
		$db = MKSDC_DataAccess::instance();
		$r = new stdClass();
		$ds = $db->getDataset(get_the_ID());
		$dp = $ds['post'];
		$r->{"@type"} = "dcat:CatalogRecord";
		$r->{"@id"} = "record:" . $dp->post_name;
		$r->{"title"} = $dp->post_title;
		$r->{"description"} = get_the_excerpt();
		$r->{"issued"} = $dp->post_date_gmt;
		$r->{"modified"} = $dp->post_modified_gmt;
		$r->{"primaryTopic"} =  self::_dataset($ds);
		return $r;
	}
	
	public static function dataset($post_name){
		$d = get_page_by_path( $post_name, OBJECT, 'mksdc-datasets');
		$o = self::_dataset($d->ID);
		$o->{"@context"} = self::_context();
		return $o;
	}
	
	private static function _dataset($ds){
		$db = MKSDC_DataAccess::instance();
		if(!is_array($ds)){
			$ds = $db->getDataset($ds);
		}
		
		$d = new stdClass();
		$dp = $ds['post'];
		$d->{"@type"} = "dcat:Dataset";
		$d->{"@id"} = "dataset:" . @$ds['uuid'][0];
		$d->{"title"} = $dp->post_title;
		$d->{"description"} = get_the_excerpt();
		$d->{"landingPage"} = get_permalink($dp->ID);
		$d->{"homepage"} = get_permalink($dp->ID);
		$d->{"name"} = $dp->post_name;
		$d->{"attribution"} = $ds['attribution'];
		$d->{"uuid"} = @$ds['uuid'][0];
		$d->{"api"} = MKSDC_Plugin::getDatasetApiLink($dp->post_name);
		$d->{"issued"} = $dp->post_date_gmt;
		$d->{"modified"} = $dp->post_modified_gmt;

		if(is_array($ds['owner'])){
			$owners = array();
			foreach($ds['owner'] as $owner){
				array_push($owners, $owner->name);
			}
			$d->{"cat:owner"} = $owners;
		}
		if(is_array($ds['formats'])){
			$formats = array();
			foreach($ds['formats'] as $form){
				array_push($formats, $form->name);
			}
			$d->{"cat:format"} = $formats;
		}
		$categories = array();
		foreach($ds['categories'] as $ca){
			$c = new stdClass();
			$c->{"skos:prefLabel"} = $ca->name;
			$c->{"@id"} = "category:" . $ca->slug;
			$c->{"@type"} = "skos:Concept";
			array_push($categories, $c);
		}
		$d->{"theme"} = $categories;

		if(is_array($ds['license'])){
			$licenses = array();
			foreach($ds['license'] as $ca){
				$lic = get_post($ca['id']);
				$c = new stdClass();
				$c->{"title"} = $ca['title'];
				$c->{"@type"} = "cat:RedistributionPolicy";
				$c->{"@id"} = "policy:" . $lic->post_name;
				$c->{"api"} = MKSDC_PLugin::getApiLink(array('action' => 'policy', 'id' => $lic->post_name));
				$c->{"homepage"} = get_the_permalink($ca['id']);
				$c->{"landingPage"} = get_the_permalink($ca['id']);
				array_push($licenses, $c);
			}
			$d->{"cat:policy"} = $licenses;
		}
		$d->{"services"} = self::_distributions($ds);
		return $d;
	}
	
	public static function policies(){
		$db = MKSDC_DataAccess::instance();
		$args=array(
				'post_type' => 'mksdc-policies',
				'post_status' => 'publish',
				'ignore_sticky_posts'=> 1
		);
		$my_query = new WP_Query($args);
		$o = new stdClass();
		$ctx = "@context";
		$o->$ctx = self::_context();
		$o->{"title"} = "Redistribution Policies";
		$o->{"@type"} = "cat:Policies";
		$o->{"@id"} = "ns:policies";
		$o->policies = array();
		while ($my_query->have_posts()) {
			$my_query->the_post();
			array_push($o->policies, self::_policy(get_the_ID()));
		}
		return $o;
	}
	
	public static function policy($post_name){
		$d = get_page_by_path( $post_name, OBJECT, 'mksdc-policies');
		$o = self::_policy($d->ID, TRUE);
		$o->{"@context"} = self::_context();
		return $o;
	}
	
	private static function _policy($pid, $verbose = FALSE){
		$db = MKSDC_DataAccess::instance();
		$policy = $db->getLicenseByPostId($pid);
		$d = new stdClass();
		$dp = $policy['post'];
		$d->{"@type"} = "cat:RedistributionPolicy";
		$d->{"@id"} = "policy:" . $dp->post_name;
		$d->{"title"} = $dp->post_title;
		$d->{"description"} = get_the_excerpt();
		$d->{"landingPage"} = get_permalink($dp->ID);
		$d->{"name"} = $dp->post_name;
		$d->{"api"} = MKSDC_PLugin::getApiLink(array('action' => 'policy', 'id' => $dp->post_name));
		if($verbose){
			$taxonomies = wp_get_post_terms($pid, array(
				// policies
				'mksdc-policy-scope',
				'mksdc-permissions',
				'mksdc-prohibitions',
				'mksdc-requirements'));
			
			$d->permissions = array();
			$d->prohibitions = array();
			$d->duties = array();
			$d->scopes = array();
			foreach($taxonomies as $c){
				if(isset($c->taxonomy)){
					$t = $c->taxonomy;
					// We create a URI for the permission/prohibition/duty
					// We don't use the action directly, following the ODRL pattern:
					// policy duty aDuty . aDuty a Duty ; action aAction .
					$localId = $c->slug . '-' . $pid;
					// if permission
					if($t == 'mksdc-permissions'){
						$_p = new stdClass();
						$_p->{"@type"} = "odrl:Permission";
						$_p->{"@id"} = "permission:" . $localId;
						$_p->{"action"} = "act:" . $c->slug;
						array_push($d->permissions, $_p);
					}else
						// if requirement
						if($t == 'mksdc-requirements'){
						$_p = new stdClass();
						$_p->{"@type"} = "odrl:Duty";
						$_p->{"@id"} = "duty:" . $localId;
						$_p->{"action"} = "act:" . $c->slug;
						array_push($d->duties, $_p);
					}else
						// if prohibition
						if($t == 'mksdc-prohibitions'){

						$_p = new stdClass();
						$_p->{"@type"} = "odrl:Prohibition";
						$_p->{"action"} = "act:" . $c->slug;
						$_p->{"@id"} = "prohibition:" . $localId;
						array_push($d->prohibitions, $_p);
					}else
						// if scope
						if($t == 'mksdc-policy-scope'){
						array_push($d->scopes, "scope:" . $c->slug);
						
					}
				}
			}
			
		}
		return $d;
	}
	
	private static function _distributions($ds){
		$db = MKSDC_DataAccess::instance();
		if(!is_array($ds)){
			$ds = $db->getDataset($ds);
		}
		$ss = $ds['services'];
		$services = array();
		foreach($ss as $kk => $vv){
			// XXX Ignore services without specified URL
			if(@$vv['url']){
				$s = new stdClass();
				$s->{"@type"} = "dcat:Distribution";
				$s->{"@id"} = "distribution:" . crc32(@$vv['url'] . $ds['post']->ID);
				$s->{"title"} = $vv['title'];
				$s->{"dcat:accessURL"} = @$vv['url'];
				array_push($services, $s);
			}
		}
		return $services;
	}
}