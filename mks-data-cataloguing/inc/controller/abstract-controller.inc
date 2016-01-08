<?php 
require_once(dirname(__FILE__) . '/../mvctools.inc');
require_once(dirname(__FILE__) . '/../dataaccess.inc');

class MKSDC_AbstractController extends MVC_Controller{
	
	public function __construct($args){
		$process = $args['parameters'];
		foreach ($process as $k => $v) {
			if (is_array($v)) {
				$nv = array();
				foreach($v as $vk => $vv){
					if(is_array($vv)) {
						$nv[stripslashes($vk)] = $vv; // XXX We might stripslashes recursively...
						continue;
					}
					$nv[stripslashes($vk)] = stripslashes($vv);
				}
				$process[stripslashes($k)] = $nv;
			} else {
				$process[stripslashes($k)] = stripslashes($v);
			}
		}
		$args['parameters'] = $process;
		parent::__construct($args);
	}
	
	protected function _notFound($msg = "Not Found", $title = "Not Found"){
		wp_die($msg, $title, 404);
		return;
	}
	
	protected function _error($msg = "Internal Server Error", $title = "Internal Server Error"){
		wp_die($msg, $title, 500);
		return;
	}
	
	protected function _badRequest($msg = "Bad Request", $title = "Bad Request"){
		wp_die($msg, $title, 400);
		return;
	}
	
	protected function _forbidden($msg = "Forbidden", $title = "Forbidden"){
		wp_die($msg, $title, 403);
		return;
	}
	
	/**
	 * Adds parameters to the current page
	 * @param array $params
	 */
	protected function redirectToMyselfWith(array $params){
		$redirectto = $_SERVER['REQUEST_URI'];
		if(strpos($_SERVER['REQUEST_URI'], '?')!==FALSE){
			$redirectto = $_SERVER['REQUEST_URI'] . '&';
		}else{
			$redirectto = $_SERVER['REQUEST_URI'] . '?';
		}
		$pars = array();
		foreach($params as $par => $val){
			array_push($pars, $par . '=' . urlencode($val));
		}
		$querystring = join('&', $pars);
		wp_redirect($redirectto . $querystring);
	}
}

