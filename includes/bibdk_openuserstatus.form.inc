<?php

/**
 * @param $form
 * @return string
 */
function bibdk_openuserstatus_form($form) {
  $response = bibdk_openuserstatus_request();
  //TODO mmj do error check
  $form = bibdk_openuserstatus_get_tables($form, $response);
  return $form;
}

/**
 * @param $response
 * @param $form
 * @return string
 */
function bibdk_openuserstatus_get_tables($form, $response) {
  //TODO mmj create basic info -- @see theme/bibdk_openuserstatus.tpl.php
  $form = bibdk_openuserstatus_get_loans_table($form, $response);

  $reservations = bibdk_openuserstatus_get_reservations($response);
  $form = bibdk_openuserstatus_get_reservations_readyforpickup_table($form, $reservations);
  $form = bibdk_openuserstatus_get_reservations_inprocess_table($form, $reservations);

  $form = bibdk_openuserstatus_get_fiscal_account_table($form, $response);

  return $form;
}

/**
 * @param $response
 * @return string
 */
function bibdk_openuserstatus_get_reservations($response) {
  $reservations = array();
  foreach ($response as $id => $favourite) {
    if (is_object($favourite)) {
      $resp = $favourite->getResponse();
      if (isset($resp['orderedItems']['order'])) {
        foreach ($resp['orderedItems']['order'] as $orderedItem) {
          $orderedItem['agencyId'] = $id;
          $reservations[] = $orderedItem;
        }
      }
    }
  }
  return $reservations;
}

/**
 * @param $form
 * @param $response
 * @return array
 */
function bibdk_openuserstatus_get_loans_table($form, $response) {
  $loans = bibdk_openuserstatus_get_loans($response);

  $data['items'] = array();
  if (!empty($loans)) {
    $data['items'] = bibdk_openuserstatus_get_loaned_items($loans);
  }

  $data['header'] = array(
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'returndate' => t('returndate_header', array(), array('context' => 'bibdk_openuserstatus')),
  );
  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  return bibdk_openuserstatus_get_loans_table_form($form, $data);
}

/**
 * @param $response
 * @return string
 */
function bibdk_openuserstatus_get_loans($response) {
  $loans = array();
  foreach ($response as $id => $favourite) {
    if (is_object($favourite)) {
      $resp = $favourite->getResponse();
      if (isset($resp['loanedItems']['loan'])) {
        foreach ($resp['loanedItems']['loan'] as $loanedItem) {
          $loanedItem['agencyId'] = $id;
          $loans[] = $loanedItem;
        }
      }
    }
  }
  return $loans;
}

/**
 * @param $loans
 * @return array
 */
function bibdk_openuserstatus_get_loaned_items($loans) {
  $items = array();
  foreach ($loans as $material) {
    $item = array();
    $loanId = $material['loanId'];
    $item['material'] = $material['author'] . ': ' . $material['title'];
    $item['library'] = bibdk_openuserstatus_get_agencyname($material['agencyId']);
    $item['returndate'] = date('d.m.Y', strtotime($material['dateDue']));
    $items[$loanId] = $item;
  }
  return $items;
}

/**
 * @param $form
 * @param $data
 * @return mixed
 */
function bibdk_openuserstatus_get_loans_table_form($form, $data) {
  $loans = t('Loans', array(), array('context' => 'bibdk_userstatus'));
  $loansCount = (!empty($data['items'])) ? $loans . ' (' . count($data['items']) . ')' : $loans;

  $form['loans']['loans_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $loansCount . '</h2>'
  );

  $form['loans']['loans_table'] = array(
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
    $form['loans']['loans_renew_all'] = array(
      '#type' => 'submit',
      '#value' => t('renew_all', array(), array('context' => 'bibdk_openuserstatus')),
      '#submit' => array('bibdk_openuserstatus_renew_all_submit'),
    );

    $form['loans']['loans_renew_marked'] = array(
      '#type' => 'submit',
      '#value' => t('renew_marked', array(), array('context' => 'bibdk_openuserstatus')),
      '#submit' => array('bibdk_openuserstatus_renew_marked_submit'),
    );
  }

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function bibdk_openuserstatus_renew_marked_submit($form, &$form_state) {
  dpm($form_state, 'bibdk_openuserstatus_renew_marked_submit');
}

/**
 * @param $form
 * @param $form_state
 */
function bibdk_openuserstatus_renew_all_submit($form, &$form_state) {
  dpm($form_state, 'bibdk_openuserstatus_renew_all_submit');
}

/**
 * @param $form
 * @param $reservations
 * @return array
 */
function bibdk_openuserstatus_get_reservations_inprocess_table($form, $reservations) {
  $data['items'] = array();
  if (!empty($reservations)) {
    $data['items'] = bibdk_openuserstatus_materials_in_process($form, $reservations);
  }

  $data['header'] = array(
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'reservation_date' => t('reservation_date', array(), array('context' => 'bibdk_openuserstatus')),
    'queue_position' => t('queue_position', array(), array('context' => 'bibdk_openuserstatus')),
  );

  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  $form = bibdk_openuserstatus_get_reservations_table_form($form, $data);
  return $form;
}

/**
 * @param $form
 * @param $res
 * @return array
 */
function bibdk_openuserstatus_materials_in_process(&$form, $res) {
  $favourites = bibdk_openuserstatus_get_favourites_array();

  $items = array();
  foreach ($res as $reservation) {
    if (isset($reservation['orderStatus']) && $reservation['orderStatus'] == 'In process') {
      $item = array();
      $orderId = $reservation['orderId'];
      $item['material'] = $reservation['author'] . ': ' . $reservation['title'];
      $item['library'] = drupal_render(drupal_get_form('bibdk_openuserstatus_select', $favourites, $reservation['pickUpAgency']));
      $item['reservation_date'] = date('d.m.Y', strtotime($reservation['orderDate']));
      $item['queue_position'] = $reservation['holdQueuePosition'];
      $items[$orderId] = $item;
    }
  }
  return $items;
}

/**
 * @return array
 */
function bibdk_openuserstatus_get_favourites_array() {
  $favourites = bibdk_openuserstatus_get_favourites();
  $arr = array();
  foreach ($favourites as $favourite) {
    $arr['DK-' . $favourite->getBranch()->branchId] = $favourite->getBranch()->branchName;
  }
  return $arr;
}

/**
 * Returns a form array representing a select box. This is done in a
 * drupal_get_form to ensure the #default_value functionality
 *
 * @param $form
 * @param $form_state
 * @param $lib
 * @param $default
 * @return array
 */
function bibdk_openuserstatus_select($form, &$form_state, $lib, $default) {
  $form['select'] = array(
    '#type' => 'select',
    '#options' => $lib,
    '#default_value' => $default,
  );
  return $form;
}

/**
 * @param $form
 * @param $data
 * @return mixed
 */
function bibdk_openuserstatus_get_reservations_table_form($form, $data) {
  $reservations = t('Reservations_in_process', array(), array('context' => 'bibdk_userstatus'));
  $reservationsCount = (!empty($data['items'])) ? $reservations . ' (' . count($data['items']) . ')' : $reservations;

  $form['reservations']['reservations_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $reservationsCount . '</h2>'
  );

  $form['reservations']['reservations_table'] = array(
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
    $form['reservations']['reservations_delete'] = array(
      '#type' => 'submit',
      '#value' => t('delete_selected', array(), array('context' => 'bibdk_openuserstatus')),
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
  dpm($form_state, 'bibdk_openuserstatus_delete_selected');
}

/**
 * @param $form
 * @param $reservations
 * @return mixed
 */
function bibdk_openuserstatus_get_reservations_readyforpickup_table($form, $reservations) {
  $data['items'] = array();
  if (!empty($reservations)) {
    $data['items'] = bibdk_openuserstatus_get_ready_for_pickup_materials($reservations);
  }

  $data['header'] = array(
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'library' => t('library_header', array(), array('context' => 'bibdk_openuserstatus')),
    'pickup_date' => t('pickup_date_header', array(), array('context' => 'bibdk_openuserstatus')),
    'pickup_id' => t('pickup_id_header', array(), array('context' => 'bibdk_openuserstatus')),
  );
  $data['msg'] = t('no_loans_found', array(), array('context' => 'bibdk_openuserstatus'));

  return bibdk_openuserstatus_get_readyforpickup_table($form, $data);
}

/**
 * @param $form
 * @param $data
 * @return mixed
 */
function bibdk_openuserstatus_get_readyforpickup_table($form, $data) {
  $readyforpcikTxt = t('Reservations_readyforpickup', array(), array('context' => 'bibdk_userstatus'));
  $readyforpcikCount = (!empty($data['items'])) ? $readyforpcikTxt . ' (' . count($data['items']) . ')' : $readyforpcikTxt;

  $form['readyforpickup']['readyforpickup_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $readyforpcikCount . '</h2>'
  );

  $form['readyforpickup']['readyforpickup_table'] = array(
    '#type' => 'markup',
    '#markup' => drupal_render(bibdk_openuserstatus_get_table($data)),
  );
  return $form;
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
 * @param $form
 * @param $response
 * @return mixed
 */
function bibdk_openuserstatus_get_fiscal_account_table($form, $response) {
  $fiscals = bibdk_openuserstatus_get_fiscals($response);
  $data['items'] = array();

  if (!empty($fiscals)) {
    $data['items'] = bibdk_openuserstatus_get_fiscal_materials($fiscals);
  }

  $data['header'] = array(
    'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
    'type' => t('type_header', array(), array('context' => 'bibdk_openuserstatus')),
    'date' => t('date_header', array(), array('context' => 'bibdk_openuserstatus')),
    'amount' => t('amount_header', array(), array('context' => 'bibdk_openuserstatus')),
  );

  $data['msg'] = t('we_have_no_registered_debt_associated_with_your_name', array(), array('context' => 'bibdk_openuserstatus'));

  return bibdk_openuserstatus_get_fiscal_account_table_form($form, $data);
}

/**
 * @param $form
 * @param $data
 * @return mixed
 */
function bibdk_openuserstatus_get_fiscal_account_table_form($form, $data) {
  $fiscalBaseTxt = t('fiscal_account', array(), array('context' => 'bibdk_userstatus'));
  $fiscalTxt = (!empty($data['items'])) ? $fiscalBaseTxt . ' (' . count($data['items']) . ')' : $fiscalBaseTxt;

  $form['fiscal']['fiscal_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $fiscalTxt . '</h2>'
  );

  $form['fiscal']['fiscal_table'] = array(
    '#type' => 'markup',
    '#markup' => drupal_render(bibdk_openuserstatus_get_table($data)),
  );

  return $form;
}

/**
 * @param $response
 * @return string
 */
function bibdk_openuserstatus_get_fiscals($response) {
  $fiscals = array();
  foreach ($response as $id => $favourite) {
    if (is_object($favourite)) {
      $resp = $favourite->getResponse();
      if (isset($resp['fiscalAccount']['fiscalTransaction'])) {
        foreach ($resp['fiscalAccount']['fiscalTransaction'] as $loanedItem) {
          $loanedItem['agencyId'] = $id;
          $fiscals[] = $loanedItem;
        }
      }
    }
  }
  return $fiscals;
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
