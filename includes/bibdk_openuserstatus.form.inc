<?php

/**
 * @param $form
 * @internal param $form_state
 * @return array
 */
function bibdk_openuserstatus_form($form) {
  $res = bibdk_openuserstatus_request();
  if ($res->getError()) {
    //TODO handle error
  }

  $response = $res->getResponse();

  $form['tables'] = bibdk_openuserstatus_get_tables($response);
  return $form;
}

function bibdk_openuserstatus_get_tables($response) {
  return array(
    '#markup' => theme('bibdk_openuserstatus', array(
      'loans' => bibdk_openuserstatus_get_loans_table($response),
      'loans_count' => bibdk_openuserstatus_get_loans_count_table($response),
      'ready_for_pickup' => bibdk_openuserstatus_get_ready_for_pickup_table($response),
      'reservations' => bibdk_openuserstatus_get_reservations_table($response),
      'fiscal_account' => bibdk_openuserstatus_get_fiscal_account_table($response),
    )),
  );
}

/**
 * @param $res
 * @return array $data
 */
function bibdk_openuserstatus_get_loans_table($res) {
  $data['items'] = array();
  if (!empty($res['loanedItems']['loan'])) {
    $data['items'] = bibdk_openuserstatus_get_loaned_items($res['loanedItems']['loan']);
  }


  $data['header'] = array(
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'material' => t('material_header', array(), array('context' => 'bibdk_openuserstatus')),
    'returned' => t('returndate_header', array(), array('context' => 'bibdk_openuserstatus')),
    'mark' => t('mark_header', array(), array('context' => 'bibdk_openuserstatus')),
  );
  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  return drupal_get_form('bibdk_openuserstatus_get_loans_table_form', $data);
}

/**
 * @param $loan
 * @return array
 */
function bibdk_openuserstatus_get_loaned_items($loan) {
  $items = array();
  foreach ($loan as $material) {
    $item = array();
    $item['library'] = 'unknown';
    $item['material'] = $material['author'] . ': ' . $material['title'];
    $item['returned'] = date('d.m.Y', strtotime($material['dateDue']));
    $item['mark'] = 'checkbox';
    $items[] = $item;
  }
  return $items;
}

/**
 * @param $res
 * @return string
 */
function bibdk_openuserstatus_get_loans_count_table($res) {
  $count = '';
  if (!empty($res['loanedItems']['loansCount'])) {
    $count = '(' . $res['loanedItems']['loansCount'] . ')';
  }
  return $count;
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_get_ready_for_pickup_table($res) {
  $data['items'] = array();
  if (!empty($res['orderedItems']['order'])) {
    $data['items'] = bibdk_openuserstatus_get_ready_for_pickup_materials($res['orderedItems']['order']);
  }

  $data['header'] = array(
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'material' => t('material_header', array(), array('context' => 'bibdk_openuserstatus')),
    'pickup_date' => t('pickup_date_header', array(), array('context' => 'bibdk_openuserstatus')),
    'pickup_id' => t('pickup_id_header', array(), array('context' => 'bibdk_openuserstatus')),
  );
  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  return bibdk_openuserstatus_get_table($data);
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_get_ready_for_pickup_materials($res) {
  $items = array();
  foreach ($res as $reservation) {
    if(isset($reservation['orderStatus']) && $reservation['orderStatus'] == 'Available for pickup'){
      $item = array();
      $item['library'] = bibdk_openuserstatus_get_agencyname($reservation['pickUpAgency']);
      $item['material'] = $reservation['author'] . ': ' . $reservation['title'];
      $item['pickup_date'] = date('d.m.Y', strtotime($reservation['pickUpExpiryDate']));
      $item['pickup_id'] = $reservation['pickUpId'];
      $items[] = $item;
    }
  }
  return $items;
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_get_reservations_table($res) {
  $data['items'] = array();
  if (!empty($res['orderedItems']['order'])) {
    $data['items'] = bibdk_openuserstatus_materials_in_process($res['orderedItems']['order']);
  }

  $data['header'] = array(
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'material' => t('material_header', array(), array('context' => 'bibdk_openuserstatus')),
    'reservation_date' => t('reservation_date', array(), array('context' => 'bibdk_openuserstatus')),
    'queue_position' => t('queue_position', array(), array('context' => 'bibdk_openuserstatus')),
  );

  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  return drupal_get_form('bibdk_openuserstatus_get_reservations_table_form', $data);
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_materials_in_process($res) {
  $items = array();
  foreach ($res as $reservation) {
    if(isset($reservation['orderStatus']) && $reservation['orderStatus'] == 'In process'){
      $item = array();
      $item['library'] = bibdk_openuserstatus_get_agencyname($reservation['pickUpAgency']);
      $item['material'] = $reservation['author'] . ': ' . $reservation['title'];
      $item['reservation_date'] = date('d.m.Y', strtotime($reservation['orderDate']));
      $item['queue_position'] = $reservation['holdQueuePosition'];
      $items[] = $item;
    }
  }
  return $items;
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_get_fiscal_account_table($res) {
  $data['items'] = array();

  if (!empty($res['fiscalAccount']['fiscalTransaction'])) {
    $data['items'] = bibdk_openuserstatus_get_fiscal_materials($res['fiscalAccount']['fiscalTransaction']);
  }

  $data['header'] = array(
    'date' => t('date_header', array(), array('context' => 'bibdk_openuserstatus')),
    'material' => t('material_header', array(), array('context' => 'bibdk_openuserstatus')),
    'type' => t('type_header', array(), array('context' => 'bibdk_openuserstatus')),
    'amount' => t('amount_header', array(), array('context' => 'bibdk_openuserstatus')),
  );

  $data['msg'] = t('we_have_no_registered_debt_associated_with_your_name', array(), array('context' => 'bibdk_openuserstatus'));

  return bibdk_openuserstatus_get_table($data);
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_get_fiscal_materials($res) {
  $items = array();
  foreach ($res as $reservation) {
    $item = array();
    $item['date'] = date('d.m.Y', strtotime($reservation['fiscalTransactionDate']));
    $item['material'] = $reservation['author'] . ': ' . $reservation['title'];
    $item['type'] = $reservation['fiscalTransactionType'];
    $item['amount'] = $reservation['fiscalTransactionAmount'] . ' ' . $reservation['fiscalTransactionCurrency'];
    $items[] = $item;
  }
  return $items;
}

/**
 * @param array $data
 * @return array
 */
function bibdk_openuserstatus_get_table($data) {
  return array(
    '#theme' => 'table',
    '#header' => $data['header'],
    '#rows' => $data['items'],
    '#empty' => $data['msg'],
    '#attributes' => array(
      'class' => array(
        'table',
      ),
    ),
  );
}

/**
 * @param $form
 * @param $form_state
 * @param array $data
 * @return array $form
 */
function bibdk_openuserstatus_get_reservations_table_form($form, &$form_state, $data) {
  $form['table'] = array(
    '#type' => 'tableselect',
    '#header' => $data['header'],
    '#options' => $data['items'],
    '#empty' => $data['msg'],
    '#js_select' => FALSE,
    '#sticky' => FALSE,
    '#multiple' => TRUE,
    '#attributes' => array(
      'class' => array(
        'table',
      ),
    ),
  );

  if(!empty($data['items'])){
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('delete_selected', array(), array('context' => 'bibdk_openuserstatus')),
      '#executes_submit_callback' => FALSE,
      '#submit' => array('bibdk_openuserstatus_renew_all_submit'),
    );
  }

  return $form;
}

/**
 * @param $form
 * @param $form_state
 * @param array $data
 * @return array $form
 */
function bibdk_openuserstatus_get_loans_table_form($form, &$form_state, $data) {
  $form['table'] = array(
    '#type' => 'tableselect',
    '#header' => $data['header'],
    '#options' => $data['items'],
    '#empty' => $data['msg'],
    '#js_select' => FALSE,
    '#sticky' => FALSE,
    '#multiple' => TRUE,
    '#attributes' => array(
      'class' => array(
        'table',
      ),
    ),
  );

  if(!empty($data['items'])){
    $form['renew_all'] = array(
      '#type' => 'submit',
      '#value' => t('renew_all', array(), array('context' => 'bibdk_openuserstatus')),
      '#executes_submit_callback' => FALSE,
      '#submit' => array('bibdk_openuserstatus_renew_all_submit'),
    );

    $form['renew_marked'] = array(
      '#type' => 'submit',
      '#value' => t('renew_marked', array(), array('context' => 'bibdk_openuserstatus')),
      '#executes_submit_callback' => FALSE,
      '#submit' => array('bibdk_openuserstatus_renew_marked_submit'),
    );
  }

  return $form;
}

function bibdk_openuserstatus_renew_all_submit() {
  drupal_goto('/');
}

function bibdk_openuserstatus_renew_marked_submit() {
  drupal_goto('/');
  dpm('bibdk_openuserstatus_renew_marked_submit');
}