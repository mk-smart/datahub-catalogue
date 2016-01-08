<?php
/**
 * MVC TOOLS
 *
 * This is a light MVC framework.
 * It is unlicensed and free.
 *
 * @author enridaga
 *
 */
class MVC_Container {
	protected $_parameters = array();
	public function setParam($key, $value){
		$this->_parameters[$key] = $value;
	}

	public function getParam($key, $default = FALSE){
		return $this->get($key, $default);
	}

	public function has($key){
		return isset($this->_parameters[$key]);
	}

	public function full($key){
		if (!$this->has($key)) return FALSE;
		$v = $this->get($key);
		if (is_array($v)) {
			return count($v) != 0;
		} else {
			return $v != '';
		}
	}

	public function get($key, $default = FALSE){
		return $this->has($key) ? $this->_parameters[$key] : $default;
	}

	public function getAll(){
		return $this->_parameters;
	}

	/**
	 *
	 * @param unknown $param
	 * @param unknown $itemKey
	 * @param string $default
	 */
	public function itemOf($key,$itemKey, $default = NULL){
		if($this->has($key)){

			$a = $this->get($key);
			if(is_array($a) && array_key_exists($itemKey, $a)){

				return $a[$itemKey];
			}elseif(is_object($a) && property_exists($a, $itemKey)){
				return $a->$itemKey;
			}
		}
		return $default;
	}
}
abstract class MVC_Controller extends MVC_Container{

	protected $_action_parameter = 'action';
	// 	protected $_parameters = array();
	protected $_validators = array();
	protected $_sanitizers = array();
	public function __construct($args = array()){
		if(isset($args['action_parameter'])){
			$this->_action_parameter = $args['action_parameter'];
		}
		if(isset($args['parameters'])){
			$this->_parameters = $args['parameters'];
		}
		// Register shared validators
		$this->setValidator('NotNull', new MVC_NotNullVal());
		$this->setValidator('NotEmpty', new MVC_NotEmptyVal());
		$this->setValidator('Int', new MVC_IntVal());
	}

	public function indexAction(){
		// Nothing to do
	}

	public function run(){
		if(isset($this->_parameters[$this->_action_parameter])){
			$this->perform($this->_parameters[$this->_action_parameter]);
		}else{
			$this->perform('index');
		}
	}

	public function perform($action){
		$method = $action . 'Action';
		if(method_exists($this, $method)){
			$this->$method();
		}else{
			$this->indexAction();
		}
	}

	public function setValidator($name, MVC_Validator $validator){
		$this->_validators[$name] = $validator;
	}

	public function setSanitizer($name, MVC_Sanitizer $sanitizer){
		$this->_sanitizers[$name] = $sanitizer;
	}

	public function getValidator($name){
		return $this->_validators[$name];
	}

	public function getSanitizer(){
		return $this->_sanitizers[$name];
	}

	private function addToOrCreateList (&$list, &$item) {
		if (!isset($list)) $list = array();
		if (is_array($item)) {
			foreach ($item as $v) { $list[] = $v; }
		} else {
			$list[] = $item;
		}
	}

	public function validate($validators){
		#echo "<!--\nIn validate...\nVALIDATORS:\n";print_r($validators);print "\nPARAMETERS:\n";print_r($this->_parameters);echo "\n-->\n";
		$errors = array();
		// XXX We want validators to check also missing parameters! -- enridaga
		foreach($validators as $param => $theValidators){
			#echo "<!-- PARAM: $param -->\n";
			if(isset($this->_parameters[$param])){
				$value = $this->_parameters[$param];
			}else{
				$value = NULL;
			}
			$isArray = is_array($value);
			# Make sure the value is an array as it is sometimes so it saves
			# special cases.
			if (!$isArray) $value = array($value);
			// 			if(isset($validators[$param])){
			#echo "<!-- Has validator(s)! -->\n";
			#echo "<!-- IS ARRAY? [$isArray] -->\n";
			foreach ($theValidators as $validator) {
				$tester = $validator instanceof MVC_Validator ?
									$validator : $this->getValidator($validator);
				# If the validator is a list validator...
				if ($tester instanceof MVC_List_Validator) {
					#echo "<!--\nFound an MVC_List_Validator with the value...\n"; print_r($value); echo "\n-->\n";
					# Only do something if the parameter is a list
					if ($isArray) {
						#echo "<!--\nFound an MVC_List_Validator and a list to validate!\n"; print_r($value); echo "\n-->\n";
						# Run the validator and get the result
						$result = $tester->test($value);
						# If all is not OK (i.e. if validation failed)...
						if ($result !== TRUE) {
							# If an array comes back, then treat each item as specific
							# feedback for that element.
							if (is_array($result)) {
								foreach ($result as $i => $v) {
									# Add the error to the list for that field
									self::addToOrCreateList($errors[$param."[$i]"], $v);
								}
							} else {
								foreach ($value as $i => $v) {
									# Add the error to the list for that field
									self::addToOrCreateList($errors[$param."[$i]"], $result);
								}
							}
						}
					}
				}
				# If the validator is a scalar validator...
				else {
					# For each item in the array...
					foreach ($value as $i => $v) {
						# Run the validator and get the result
						$result = $tester->test($v);
						# If all is not OK (i.e. if validation failed)...
						if ($result !== TRUE) {
							# If the original value was an array then it'll need square
							# brackets; otherwise it won't.
							$p = $param;
							if ($isArray) $p .= '[' . $i . ']';
							# Add the error to the list for that field
							self::addToOrCreateList($errors[$p], $result);
						}
					}
				}
			}
			// 			}
		}
		return $errors;
	}

	private function applySan($result, $san){
		if($san instanceof MVC_Sanitizer){
			$result = $san->filter($result);
		}else if(function_exists($san)){
			$result = @$san($result);
		}else{
			$sanitizer = $this->getSanitizer($san);
			$result = $sanitizer->filter($result);
		}
		return $result;
	}

	public function sanitize($sanitizers){
		$errors = array();
		foreach($this->_parameters as $param => $value){
			if(isset($sanitizers[$param])){
				$result = $value;
				if(is_array($sanitizers[$param])){
					foreach($sanitizers[$param] as $san){
						$result = $this->applySan($result, $san);
					}
				}else{
					$result = $this->applySan($result, $sanitizers[$param]);
				}
				$this->_parameters[$param] = $result;
			}
		}
	}
}


interface MVC_Sanitizer{
	public function filter($val);
}

interface MVC_Validator{
	public function test($val);
}

interface MVC_List_Validator extends MVC_Validator {
	# It doesn't really - it's just a way of identifying whether it expects a
	# list or a scalar.
}

class MVC_OneOfVal implements MVC_Validator{
	private $_mode = NULL;
	private $_values = NULL;
	public function __construct($values, $mode = TRUE){
		$this->_values = $values;
		$this->_mode = $mode;
	}
	public function test($val){
		if (in_array($val, $this->_values) === $this->_mode)
			return TRUE;
		return 'Value ' . ( $this->_mode ? 'not in' : ' in ' ) . ' "' . join(', ', $this->_values) . '"';
	}
	public function allowed(){
		$this->_mode = TRUE;
	}
	public function forbidden(){
		$this->_mode = FALSE;
	}
}

class MVC_CallbackVal implements MVC_Validator{
	private $_function = NULL;
	public function __construct($func){
		if(function_exists($func)){
			$this->_function = $func;
		} else throw new Exception('Function ' . $func . ' does not exist!');
	}

	public function test($val){
		return $func($val);
	}
}
class MVC_NotNullVal implements MVC_Validator{
	public function test($value){
		return $value === NULL ? 'Cannot be null.' : TRUE;
	}
}
class MVC_NotEmptyVal implements MVC_Validator{
	public function test($value){
		return $value === '' ? 'A value is required.' : TRUE;
	}
}
class MVC_IntVal implements MVC_Validator{
	public function test($value){
		return (strval(intval($value)) === $value) ?
					TRUE :
					'Must be a whole number but was: "' . htmlspecialchars($value) . '"';
	}
}

/**
 *
 * @author ed4565
 *
 */
class MVC_Viewer extends MVC_Container{

	private $_templateDirectories;

	/**
	 *
	 * @param array $data
	 * @param string|array $templateDirectory
	 */
	public function __construct ( $data = array(), $templateDirectory = NULL){
		$this->_templateDirectories = array();
		array_push($this->_templateDirectories, dirname(__FILE__) . '/html');
		if( $templateDirectory !== NULL){
			if(!is_array($templateDirectory)){
				$templateDirectory = array($templateDirectory);
			}
			foreach($templateDirectory as $t){
				array_unshift($this->_templateDirectories, $t);
			}
		}

		$this->_parameters = $data;
	}

	protected function templateDirectories(){
		return 	array_values($this->_templateDirectories);
	}

	public function template($template){
		if(file_exists($template)){
			return $template;
		}
		// Search in template directories
		foreach($this->templateDirectories() as $t){
			$f = $t . '/' . $template . '.phtml';
			if(file_exists($f)){
				return $f;
			}
		}
		return NULL;
	}

	public function render($template){
		include $this->template($template);
	}

	public function partial($template, $data = array(), $base = NULL){
		if($base == NULL){
			$base = $this->templateDirectories();
		}
		$class = get_class($this);
		$viewer = new $class($data, $base);
		$viewer->render($template);
	}

	/**
	 * Prints the value of $key
	 * @param string $key
	 */
	public function spit($key){
		print $this->get($key);
	}

	/**
	 * Prints the value of $key, encode special chars as HTML entities
	 * @param string $key
	 */
	public function smushed($key){
		if ($this->has("$key-safe")) {
			print $this->get("$key-safe");
		} else {
			print $this->smush($this->get($key));
		}
	}

	/**
	 * Returns the passed value, encode special chars as HTML entities
	 * @param string $value
	 */
	public static function smush($value){
		return htmlspecialchars($value);
	}

	public function spitMany(array $keys = array(), $sep = ' '){
		$first = TRUE;
		foreach($kets as $key){
			if($first) $first = false;
			else print $sep;
			$this->spit($key);
		}
	}

	public function placeholder ($text, $was = NULL) {
		if ($was !== NULL) {
			if ($was === '') {
				$text .= ' (was empty)';
			} else {
				$text .= ' (was: "' . $was . '")';
			}
		}
		return $text;
	}

}

class MVC_Html extends MVC_Viewer{
	public function spitAttr($key, $attr = ''){
		if($this->has($key)){
			if($attr == ''){
				$attr = $key;
			}
			print ' ' . $attr .'="';
			$this->smushed($key);
			print '"';
		}
	}

	public function spitAttrs(array $attrs = array('id', 'name', 'placeholder', 'class', 'style', 'type', 'size', 'value', 'selected', 'checked', 'rel', 'target', 'onclick', 'onfocus', 'onblur', 'onchange', 'required', 'disabled'), $prefix = ''){
		foreach($attrs as $attr){
			$_attr = $prefix . $attr;
			$this->spitAttr($_attr, $attr);
		}
	}
}
