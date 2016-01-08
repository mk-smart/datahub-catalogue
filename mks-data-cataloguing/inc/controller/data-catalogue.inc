<?php 
require_once(dirname(__FILE__) . '/abstract-controller.inc');

class MKSDC_DataCatalogue_Controller extends MKSDC_AbstractController{
	public function __construct($args){
		if(session_id() == '') {
			session_start();
		}
		// XXX ---> This is a public page <-------
		// 		if(!is_user_logged_in()){
		// 			auth_redirect();
		// 		}
		// 		// XXX In any case force authentication
		// 		if(class_exists('phpCAS')){
		// 			phpCAS::forceAuthentication();
		// 		}
			parent::__construct($args);
	}

	public function indexAction(){
		$db = MKSDC_DataAccess::instance();
		$model = array();
		$model['db'] = $db;
		// XXX Let's pass all the parameters as we will do the WP thing in the view this time.
		$model['params'] = $this->getAll();
		$v = new MVC_Viewer($model);
		$v->render('datasets');
	}
}
$c = new MKSDC_DataCatalogue_Controller(array(
		'action_parameter' => 'mksdc_nds_action',
		'parameters' => array_merge($_GET, $_POST)
));
$c->run();
