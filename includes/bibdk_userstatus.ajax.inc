<?php

/**
 * Menu callback
 */
function bibdk_openuserstatus_new_pickuplibrary_selected() {
  if(!isset($_POST) && empty($_POST) || !bibdk_openuserstatus_check_post_values($_POST)){
    $msg = t('insufficient_data_provided', array(), array('context' => 'bibdk_openuserstatus'));
    bibdk_openuserstatus_output_error($msg);
  }

  $post = $_POST;

  $orderId = $post['oid'];
  $agencyId = $post['aid'];
  $currentAgencyId = $post['caid'];

  if(!bibdk_openuserstatus_validate_selection()){
    $msg = t('Could not validate the data received from client', array(), array('context' => 'bibdk_openuserstatus'));
    bibdk_openuserstatus_output_error($msg);
  }

  $order = array();
  $order['orderId'] = $orderId;
  $order['pickupAgency'] = $agencyId;
  $order['agencyId'] = $currentAgencyId;

  $favourites = bibdk_openuserstatus_get_favourites();
  if(isset($favourites[$currentAgencyId])){
    $favourite = $favourites[$currentAgencyId];
  } else  {
    $msg = t('Could not validate the data received from client', array(), array('context' => 'bibdk_openuserstatus'));
    bibdk_openuserstatus_output_error($msg);
  }

  $resp = $favourite->updateOrder($order);
  fb($resp);
  return drupal_json_output('hest');
}

/**
 * @return bool
 */
function bibdk_openuserstatus_validate_selection() {
  //TODO complete this validation. Validate against current userstatus
  return TRUE;
}

/**
 * Checks for expected values in $_POST array. Returns false if a value is
 * missing otherwise TRUE is returned
 *
 * @param array $post
 * @return bool
 */
function bibdk_openuserstatus_check_post_values($post) {
  $keys = array('aid', 'oid', 'caid');
  foreach ($keys as $key) {
    if(!isset($post[$key]) && empty($post[$key])){
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Outputs the error message received as json and exits the program with code 0
 *
 * @param $msg
 */
function bibdk_openuserstatus_output_error($msg) {
  $data['error']['msg'] = $msg;
  drupal_json_output($data);
  exit(0);
}