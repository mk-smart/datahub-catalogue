<?php
require_once(dirname(__FILE__) . '/abstract-controller.inc');

class MKSDC_Api_Controller extends MKSDC_AbstractController{
	
	function indexAction(){
		$this->datasetsAction();
	}
	
	function datasetsAction(){
		$o = MKSDC_JSONLD::catalogue();
		header("Content-type: application/json; charset:utf-8");
		print json_encode($o, JSON_PRETTY_PRINT);
	}
	
	function policiesAction(){
		$o = MKSDC_JSONLD::policies();
		header("Content-type: application/json; charset:utf-8");
		print json_encode($o, JSON_PRETTY_PRINT);
	}
	
	function datasetAction(){
		$o = MKSDC_JSONLD::dataset($this->get('id'));
		header("Content-type: application/json; charset:utf-8");
		print json_encode($o, JSON_PRETTY_PRINT);
	}
	
	function policyAction(){
		$o = MKSDC_JSONLD::policy($this->get('id'));
		header("Content-type: application/json; charset:utf-8");
		print json_encode($o, JSON_PRETTY_PRINT);
	}
	
	
	function syncAction(){
		if(!is_admin()){
			return $this->_forbidden("Forbidden");
		}
		$post = get_page_by_path( $this->get('id'), OBJECT, $this->get('type')); 
		print mksdc_notify_rdf_catalogue($post->ID, $this->get('type'), $this->get('do'));
	}
}

$c = new MKSDC_Api_Controller(array(
		'action_parameter' => 'action',
		'parameters' => array_merge($_GET, $_POST)
));
$c->run();