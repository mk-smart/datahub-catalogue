<?php
require_once (dirname ( __FILE__ ) . '/generic-dataset-controller.inc');
class MKSDC_Dataset_Controller extends MKSDC_Generic_Dataset_Controller {

	/** Override in other dataset types **/
  protected function _managedType(){
		return 'file'; // Default Dataset Type
	}


	public function __construct($args) {
		if (session_id () == '') {
			session_start ();
		}
		parent::__construct ( $args );
	}
}
$c = new MKSDC_Dataset_Controller ( array (
		'action_parameter' => 'action',
		'parameters' => array_merge($_POST, $_GET)
) );
$c->run ();
