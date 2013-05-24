<?php

/**
 * @param $form
 * @internal param $form_state
 * @return array
 */
function bibdk_openuserstatus_form() {
  $res = bibdk_openuserstatus_request();
  if ($res->getError()) {
    //TODO handle error
  }

  $response = $res->getResponse();
  return $response;
}

function bibdk_openuserstatus_get_tables($form, &$form_state) {
  $response = bibdk_openuserstatus_form();

  $form['userstatus'] = array(
    '#type' => 'markup',
    '#markup' => bibdk_openuserstatus_get_markup($response, $form),
  );

  return $form;
  /*
  bibdk_openuserstatus_form(NULL);
  #$a = bibdk_openuserstatus_get_loans_table($response);
  $a = theme('bibdk_openuserstatus_table', array('table' => bibdk_openuserstatus_get_loans_table($response)));
  $b = theme('bibdk_openuserstatus_table', array('table' => bibdk_openuserstatus_get_reservations_table($response)));
  #dpm($a, 'a');
  #dpm($b, 'b');


  $arr =  array(
    '#markup' => theme('bibdk_openuserstatus', array(
      'tables' => array(
        theme('bibdk_openuserstatus_table', array('table' => bibdk_openuserstatus_get_ready_for_pickup_table($response))),
        $a,

        $b,
        theme('bibdk_openuserstatus_table', array('table' => bibdk_openuserstatus_get_fiscal_account_table($response))),
      ),
    )),
  );
  return $arr;*/
}

function bibdk_openuserstatus_get_markup($response, $form) {
  $loans = bibdk_openuserstatus_get_loans($response);
  $loansTable = bibdk_openuserstatus_get_loans_table($loans);
  return 'hest';
}

function bibdk_openuserstatus_get_loans($response) {
  return $response['loanedItems']['loan'];
}

/**
 * @param $res
 * @return array $data
 */
function bibdk_openuserstatus_get_loans_table($res = NULL) {
  $data['items'] = array();
  if (!empty($res['loanedItems']['loan'])) {
    $data['items'] = bibdk_openuserstatus_get_loaned_items($res['loanedItems']['loan']);
  }

  $data['header'] = array(
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'returndate' => t('returndate_header', array(), array('context' => 'bibdk_openuserstatus')),
  );
  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  return bibdk_openuserstatus_get_loans_table_form($data);
}

/**
 * @param $data
 * @param $res
 * @return string
 */
/*
function bibdk_openuserstaus_theme_loans_table_form($data, $res) {
  $form = drupal_get_form('bibdk_openuserstatus_get_loans_table_form', $data);

  $table = $form;
  #$actions = theme('bibdk_openuserstatus_table_actions', array('actions' => array($form['renew_marked'])));
  #$count = bibdk_openuserstatus_get_loans_count($res);
  return theme('bibdk_openuserstatus_table', array('table' => $table));
} */

/**
 * @param $loan
 * @return array
 */
function bibdk_openuserstatus_get_loaned_items($loan) {
  $items = array();
  foreach ($loan as $material) {
    $item = array();
    $item['material'] = $material['author'] . ': ' . $material['title'];
    $item['library'] = 'libraryname not provided';
    $item['returndate'] = date('d.m.Y', strtotime($material['dateDue']));
    $items[] = $item;
  }
  return $items;
}

/**
 * @param $res
 * @return string
 */
function bibdk_openuserstatus_get_loans_count($res) {
  $count = '';
  if (!empty($res['loanedItems']['loansCount'])) {
    $count = '(' . $res['loanedItems']['loansCount'] . ')';
  }
  return $count;
}

/**
 * @param array $data
 * @return array $form
 */
function bibdk_openuserstatus_get_loans_table_form($data) {
  $form['loans']['name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>Loans</h2>'
  );


  $form['loans']['table'] = array(
    '#type' => 'tableselect',
    '#header' => $data['header'],
    '#options' => $data['items'],
    '#empty' => $data['msg'],
    '#js_select' => FALSE,
    '#sticky' => FALSE,
    '#multiple' => TRUE,
    '#attributes' => array(
      'class' => array(),
    ),
  );

  if (!empty($data['items'])) {
    /*
    $form['renew_all'] = array(
      '#type' => 'submit',
      '#value' => t('delete_selected', array(), array('context' => 'bibdk_openuserstatus')),
      #'#executes_submit_callback' => FALSE,
      '#submit' => array('bibdk_openuserstatus_delete_selected'),
    );*/

    $form['loans']['renew_marked'] = array(
      '#type' => 'submit',
      '#value' => t('renew_marked', array(), array('context' => 'bibdk_openuserstatus')),
      '#submit' => array('bibdk_openuserstatus_renew_marked_submit'),
    );
  }

  return $form;
}

function bibdk_openuserstatus_get_loans_table_form_submit($form, &$form_state) {
  dpm('bibdk_openuserstatus_get_loans_table_form');
}

/**
 * @param $form
 * @param $form_state
 */
function bibdk_openuserstatus_renew_marked_submit($form, &$form_state) {
  dpm($form_state, 'bibdk_openuserstatus_renew_marked_submit');
}

function bibdk_openuserstatus_form_form_submit($form, &$form_state) {
  dpm('bibdk_openuserstatus_form_form_submit');
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
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
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
    if (isset($reservation['orderStatus']) && $reservation['orderStatus'] == 'Available for pickup') {
      $item = array();
      $item['material'] = $reservation['author'] . ': ' . $reservation['title'];
      $item['library'] = bibdk_openuserstatus_get_agencyname($reservation['pickUpAgency']);
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
function bibdk_openuserstatus_get_reservations_table($form, &$form_state, $res = NULL) {
  if (!$res) {
    $res = drupal_static('bibdk_openuserstatus_form');
  }
  $data['items'] = array();
  if (!empty($res['orderedItems']['order'])) {
    $data['items'] = bibdk_openuserstatus_materials_in_process($res['orderedItems']['order']);
  }

  $data['header'] = array(
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'reservation_date' => t('reservation_date', array(), array('context' => 'bibdk_openuserstatus')),
    'queue_position' => t('queue_position', array(), array('context' => 'bibdk_openuserstatus')),
  );

  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  $form = bibdk_openuserstatus_get_reservations_table_form($data);
  return $form;
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_materials_in_process($res) {
  $items = array();
  foreach ($res as $reservation) {
    if (isset($reservation['orderStatus']) && $reservation['orderStatus'] == 'In process') {
      $item = array();
      $item['material'] = $reservation['author'] . ': ' . $reservation['title'];
      $item['library'] = bibdk_openuserstatus_get_agencyname($reservation['pickUpAgency']);
      $item['reservation_date'] = date('d.m.Y', strtotime($reservation['orderDate']));
      $item['queue_position'] = $reservation['holdQueuePosition'];
      $items[] = $item;
    }
  }
  return $items;
}

function bibdk_openuserstatus_get_fiscal_account_table_form($form, &$form_state) {
  $form['fiscal'] = array(
    '#type' => 'markup',
    '#markup' => theme('bibdk_openuserstatus_table', array('table' => bibdk_openuserstatus_get_fiscal_account_table())),
  );
  return $form;
}

/**
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_get_fiscal_account_table($res = NULL) {
  if (!$res) {
    $res = drupal_static('bibdk_openuserstatus_form');
  }

  $data['items'] = array();

  if (!empty($res['fiscalAccount']['fiscalTransaction'])) {
    $data['items'] = bibdk_openuserstatus_get_fiscal_materials($res['fiscalAccount']['fiscalTransaction']);
  }

  $data['header'] = array(
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'type' => t('type_header', array(), array('context' => 'bibdk_openuserstatus')),
    'date' => t('date_header', array(), array('context' => 'bibdk_openuserstatus')),
    'amount' => t('amount_header', array(), array('context' => 'bibdk_openuserstatus')),
  );

  $data['msg'] = t('we_have_no_registered_debt_associated_with_your_name', array(), array('context' => 'bibdk_openuserstatus'));

  $form['fiscal'] = array(
    '#type' => 'markup',
    '#markup'
  );

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
    $item['material'] = $reservation['author'] . ': ' . $reservation['title'];
    $item['type'] = $reservation['fiscalTransactionType'];
    $item['date'] = date('d.m.Y', strtotime($reservation['fiscalTransactionDate']));
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
function bibdk_openuserstatus_get_reservations_table_form($data) {
  /*
  $form['reservations'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'element-wrapper'
      ),
    ),
  );*/

  $form['reservations']['name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>Reservations</h2>'
  );

  $form['reservations']['table'] = array(
    '#type' => 'tableselect',
    '#header' => $data['header'],
    '#options' => $data['items'],
    '#empty' => $data['msg'],
    '#js_select' => FALSE,
    '#sticky' => FALSE,
    '#multiple' => TRUE,
    '#attributes' => array(
      'class' => array(),
    ),
  );

  if (!empty($data['items'])) {
    $form['reservations']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('delete_selected', array(), array('context' => 'bibdk_openuserstatus')),
      '#executes_submit_callback' => TRUE,
      '#submit' => array('bibdk_openuserstatus_delete_selected'),
    );
  }
  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function bibdk_openuserstatus_delete_selected($form, &$form_state) {
  dpm('bibdk_openuserstatus_delete_selected');
  dpm($form_state);
}
