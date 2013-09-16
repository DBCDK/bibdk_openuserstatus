<?php

/**
 * Returns renderable form array containing tables representing the userstatus
 *
 * @param $form
 * @return array
 */
function bibdk_openuserstatus_form($form) {
  $response = bibdk_openuserstatus_request();
  $form = bibdk_openuserstatus_get_tables($form, $response);
  return $form;
}

/**
 * Collects tables for each area of the userstatus page and delivers it as one
 * single form
 *
 * @param array $response
 * @param array $form
 * @return array
 */
function bibdk_openuserstatus_get_tables($form, $response) {
  $form = bibdk_opensuser_get_basic_information($form);
  $form = bibdk_openuserstatus_get_loans_table($form, $response);

  $reservations = bibdk_openuserstatus_get_reservations($response);
  $form = bibdk_openuserstatus_get_reservations_readyforpickup_table($form, $reservations);
  $form = bibdk_openuserstatus_get_reservations_inprocess_table($form, $reservations);

  $form = bibdk_openuserstatus_get_fiscal_account_table($form, $response);

  $form['#attributes'] = array('class' => array('openuserstatus'));

  $form['#attached']['js'] = array(
    drupal_get_path('module', 'bibdk_openuserstatus') . '/js/bibdk_openuserstatus.js',
  );

  return $form;
}

/**
 * Provides basic information printet at the top of the profile2 userstatus page
 *
 * @see theme/bibdk_openuserstatus.tpl.php
 * @param $form
 * @return mixed
 */
function bibdk_opensuser_get_basic_information($form) {
  $form['basics'] = array(
    '#type' => 'markup',
    '#markup' => theme('bibdk_openuserstatus'),
  );
  return $form;
}

/**
 * Extracts reservations from each favourite library and collects them in a
 * single array
 *
 * @param array $response
 * @return array $reservations
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
 * Creates the necessary items and header arrays for the loans tableselect form
 *
 * @param array $form   The form array containing the entire form
 * @param array $response The OpenUserstatus response
 * @return array $form  The entire form array including the loans table
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

  $form = bibdk_openuserstatus_get_loans_table_form($form, $data);

  return $form;
}

/**
 * Extracts the currently loaned items from each favourite library and collects
 * them into a single array for easier parsing
 *
 * @param array $response
 * @return array $loans
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
 * Creates the items array to be used in the loaned items tableselect form
 *
 * @param array $loans
 * @return array $items
 */
function bibdk_openuserstatus_get_loaned_items($loans) {
  $items = array();
  foreach ($loans as $material) {
    $status_html = bibdk_openuserstatus_check_returndate($material['dateDue']);

    $item = array();
    $id = $material['loanId'] . ';' . $material['agencyId'];
    $material['author'] = (isset($material['author']) ? $material['author'] : '');
    $material['title'] = (isset($material['title']) ? $material['title'] : '');
    $item['material'] = theme('bibdk_openuserstatus_row_item', array('material' => $material));
    $item['library'] = bibdk_openuserstatus_get_agencyname($material['agencyId']);
    $item['returndate'] = $status_html;
    $items[$id] = $item;
  }
  return $items;
}

/** \brief Check if return date is ok.
 * return appropiate html to indicate status
 *
 * @param date $returnDate
 */
function bibdk_openuserstatus_check_returndate($returnDate) {
  // date returns FALSE on failure
  $date = date('d.m.Y', strtotime($returnDate));
  if ($date === FALSE) {
    return '';
  }
  // DateTime object throws an exception on failure
  try{
  $limit = new DateTime($date);
  }
  catch(Exceptio $e){
    // return an empty string on failure
    return '';
  }
  $limit->setTime(23, 59, 59);  // user has all day

  if ($limit < new DateTime('now')) {
    return '<p><span class="alert red-alert" title="'.bibdk_openuserstatus_get_date_overdue_txt().'"></span><span>' . $date . '</span></p>';
  }
  else{
    return '<p><span class="alert no-alert"></span><span>' . $date . '</span></p>';
  }
}

function bibdk_openuserstatus_get_date_overdue_txt() {
  $txt = t('Delivery_date_overdue', array(), array('context' => 'bibdk_openuserstatus'));
  return $txt;
}

/**
 * Returns the form array with the loaned items attached
 *
 * @param array $form
 * @param array $data
 * @return array $form
 */
function bibdk_openuserstatus_get_loans_table_form($form, $data) {
  $loans = t('Loans', array(), array('context' => 'bibdk_userstatus'));
  $loansCount = (!empty($data['items'])) ? $loans . ' <span>(' . count($data['items']) . ')</span>' : $loans;

  $form['loans']['loans_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $loansCount . '</h2>',
    '#prefix' => '<div class="element-title">',
    '#suffix' => '</div>',
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
      'class' => array('loans'),
    ),
  );

  if (!empty($data['items'])) {
    $form['loans']['loans_renew_marked'] = array(
      '#type' => 'submit',
      '#value' => t('renew_marked', array(), array('context' => 'bibdk_openuserstatus')),
      '#submit' => array('bibdk_openuserstatus_renew_marked_submit'),
    );

    $form['loans']['loans_renew_all'] = array(
      '#type' => 'submit',
      '#value' => t('renew_all', array(), array('context' => 'bibdk_openuserstatus')),
      '#submit' => array('bibdk_openuserstatus_renew_all_submit'),
    );
  }

  return $form;
}

/**
 * Creates the necessary items and header arrays for the ready_for_pickup table form
 *
 * @param array $form
 * @param array $reservations
 * @return array $form
 */
function bibdk_openuserstatus_get_reservations_inprocess_table($form, $reservations) {
  $data['items'] = array();
  if (!empty($reservations)) {
    $data['items'] = bibdk_openuserstatus_materials_in_process($reservations);
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
 * Extracts the currently loaned items from each favourite library and collects
 * them into a single array for easier parsing
 *
 * @param array $reservations
 * @return array $items
 */
function bibdk_openuserstatus_materials_in_process($reservations) {
  $items = array();
  foreach ($reservations as $reservation) {
    if (isset($reservation['orderStatus']) && ($reservation['orderStatus'] == 'In process' || $reservation['orderStatus'] == 'Requested via ill')) {

      $pickUpAgencies = bibdk_openuserstatus_get_pickup_agencies($reservation['agencyId']);

      $item = array();
      $id = $reservation['orderId'];
      $reservation['author'] = (isset($reservation['author']) ? $reservation['author'] : '');
      $reservation['title'] = (isset($reservation['title']) ? $reservation['title'] : '');
      $item['material'] = theme('bibdk_openuserstatus_row_item', array('material' => $reservation));
      if (count($pickUpAgencies) > 1) {
        $item['library']['data'] = bibdk_openuserstatus_select($pickUpAgencies, $reservation);
      }
      else {
        $item['library'] = $pickUpAgencies[$reservation['agencyId']];
      }
      $item['reservation_date'] = date('d.m.Y', strtotime($reservation['orderDate']));
      $item['queue_position'] = (isset($reservation['holdQueuePosition']) ? $reservation['holdQueuePosition'] : '');
      $items[$id] = $item;
    }
  }
  return $items;
}

/**
 * Returns a form array representing a select box. This is done in a
 * drupal_get_form to ensure the #default_value functionality
 *
 * @param $form
 * @param $form_state
 * @param $pickUpAgencies
 * @param $reservation
 * @return array $form
 */
function bibdk_openuserstatus_select($pickUpAgencies, $reservation) {
  $agencyId = $reservation['pickUpAgency'];
  if (strpos($agencyId, 'DK-') === 0) {
    $agencyId = substr($agencyId, 3, strlen($agencyId));
  }

  $pickupAgency = array_key_exists('pickUpAgencySubdivision', $reservation) ? $agencyId . '-' . $reservation['pickUpAgencySubdivision'] : $agencyId;
  $form['select'] = array(
    '#type' => 'select',
    '#options' => $pickUpAgencies,
    '#value' => $pickupAgency,
    '#attributes' => array(
      'data-oid' => array(
        $reservation['orderId'],
      ),
      'data-caid' => array(
        $reservation['agencyId'],
      ),
    ),
  );

  $form['help'] = array(
    '#markup' => '<p class="helptext" title="' . t('bibdk_openuserstatus_change_library', array(), array('context' => 'bibdk_userstatus')) . '">?</p>',
  );
  return $form;
}

/**
 * Returns the form array with the reserved items attached
 *
 * @param array $form
 * @param array $data
 * @return array $form
 */
function bibdk_openuserstatus_get_reservations_table_form($form, $data) {
  $reservations = t('Reservations_in_process', array(), array('context' => 'bibdk_userstatus'));
  $reservationsCount = (!empty($data['items'])) ? $reservations . ' <span>(' . count($data['items']) . ')</span>' : $reservations;

  $form['reservations']['reservations_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $reservationsCount . '</h2>',
    '#prefix' => '<div class="element-title">',
    '#suffix' => '</div>',
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
      'class' => array(
        'ajax-progress', 'reservations',
      ),
    ),
  );

  if (!empty($data['items'])) {
    $form['reservations']['reservations_delete'] = array(
      '#type' => 'submit',
      '#value' => t('delete_selected', array(), array('context' => 'bibdk_openuserstatus')),
      '#submit' => array('bibdk_openuserstatus_cancel_selected_submit'),
    );
  }
  return $form;
}

/**
 * Creates the necessary items and header arrays for the reservations tableselect form
 *
 * @param array $form
 * @param array $reservations
 * @return array $form
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

  $form = bibdk_openuserstatus_get_readyforpickup_table($form, $data);
  return $form;
}

/**
 * Returns form array representing the ready for pickup table
 *
 * @param array $form
 * @param array $data
 * @return array $form
 */
function bibdk_openuserstatus_get_readyforpickup_table($form, $data) {
  $readyforpcikTxt = t('Reservations_readyforpickup', array(), array('context' => 'bibdk_userstatus'));
  $readyforpcikCount = (!empty($data['items'])) ? $readyforpcikTxt . ' <span>(' . count($data['items']) . ')</span>' : $readyforpcikTxt;
  $data['class'] = 'readyforpickup';

  $form['readyforpickup']['readyforpickup_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $readyforpcikCount . '</h2>',
    '#prefix' => '<div class="element-title">',
    '#suffix' => '</div>',
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
      $reservation['author'] = (isset($reservation['author']) ? $reservation['author'] : '');
      $reservation['title'] = (isset($reservation['title']) ? $reservation['title'] : '');
      $item['material'] = theme('bibdk_openuserstatus_row_item', array('material' => $reservation));
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
  $data = array();

  if (!empty($fiscals)) {
    $data['agencies'] = bibdk_openuserstatus_get_fiscal_materials($fiscals);
  }

  $form = bibdk_openuserstatus_get_fiscal_account_table_form($form, $data);
  return $form;
}

/**
 * Returns form array representing the fiscal account table
 *
 * @param array $form
 * @param array $data
 * @return array $form
 */
function bibdk_openuserstatus_get_fiscal_account_table_form($form, $data) {

  $fiscalBaseTxt = t('fiscal_account', array(), array('context' => 'bibdk_userstatus'));
  $fiscalTxt = (!empty($data['items'])) ? $fiscalBaseTxt . ' <span>(' . count($data['items']) . ')</span>' : $fiscalBaseTxt;

  $form['fiscal']['fiscal_name'] = array(
    '#type' => 'markup',
    '#markup' => '<h2>' . $fiscalTxt . '</h2>',
    '#prefix' => '<div class="element-title">',
    '#suffix' => '</div>',
  );

  if (isset($data['agencies']) && !empty($data['agencies'])) {

    foreach ($data['agencies'] as $agencyid => $agency) {

      $agency['class'] = 'fiscal-account';

      $agency['msg'] = t('we_have_no_registered_debt_associated_with_your_name', array(), array('context' => 'bibdk_openuserstatus'));

      $agency['header'] = array(
        'material' => t('author_title', array(), array('context' => 'bibdk_openuserstatus')),
        'type' => t('type_header', array(), array('context' => 'bibdk_openuserstatus')),
        'date' => t('date_header', array(), array('context' => 'bibdk_openuserstatus')),
        'amount' => t('amount_header', array(), array('context' => 'bibdk_openuserstatus')),
      );

      $form['fiscal'][$agencyid]['branchName'] = array(
        '#type' => 'markup',
        '#markup' => '<h3>' . $agency['branchName'] . ((!empty($agency['items'])) ? ' <span>(' . count($agency['items']) . ')</span>' : '') . '</h3>',
        '#prefix' => '<div class="element-title">',
        '#suffix' => '</div>',
      );

      $form['fiscal'][$agencyid]['fiscal_table'] = array(
        '#type' => 'markup',
        '#markup' => drupal_render(bibdk_openuserstatus_get_table($agency)),
      );

      if (!empty($agency['paymentUrl']) && count($agency['items']) > 0) {
        $form['fiscal'][$agencyid]['fiscal_branch_link'] = array(
          '#type' => 'markup',
          '#prefix' => '<div class="payment-url-link">',
          '#suffix' => '</div>',
          '#markup' => l(
              t('fiscal_branch_link', array(), array('context' => 'bibdk_openuserstatus')), $agency['paymentUrl'], array(
            'attributes' => array(
              'class' => array('btn', 'btn-blue'),
              'target' => array('_blank'),
            ),
              )
          ),
        );
      }
    }
  }
  return $form;
}

/**
 * Extracts the currently fiscal items from each favourite library and collects
 * them into a single array for easier parsing
 *
 * @param array $response
 * @return array $loans
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
 * Creates the items array to be used in the fiscal account table form
 *
 * @param array $res
 * @return array $items
 */
function bibdk_openuserstatus_get_fiscal_materials($res) {
  global $language;
  $items = array();
  $favourites = bibdk_openuserstatus_get_favourites();
  global $language;
  foreach ($res as $reservation) {
    $item = array();
    $reservation['author'] = (isset($reservation['author']) ? $reservation['author'] : '');
    $reservation['title'] = (isset($reservation['title']) ? $reservation['title'] : '');
    $agencyId = (isset($reservation['agencyId']) ? $reservation['agencyId'] : NULL);
    $item['material'] = theme('bibdk_openuserstatus_row_item', array('material' => $reservation));
    $item['type'] = $reservation['fiscalTransactionType'];
    $item['date'] = date('d.m.Y', strtotime($reservation['fiscalTransactionDate']));
    $item['amount'] = $reservation['fiscalTransactionAmount'] . ' ' . $reservation['fiscalTransactionCurrency'];
    $items[$agencyId]['items'][] = $item;
    $items[$agencyId]['branchName'] = $favourites[$agencyId]->getBranch()->getBranchShortName($language->language);
    $items[$agencyId]['paymentUrl'] = $favourites[$agencyId]->getBranch()->getPaymentUrl();
  }
  return $items;
}

/**
 * Returns a renderarray representing a basic table with no interactive elements
 *
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
        'table', $data['class'],
      ),
    ),
  );
}
