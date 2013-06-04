<?php

/**
 * Implements hook_bibdk_mypage_tabs().
 */
function bibdk_openuserstatus_bibdk_mypage_tabs($max_rows) {

  global $user;

  $ret = array();
  $ret['bibdk_openuserstatus']['header'] = t('Userstatus');
  $ret['bibdk_openuserstatus']['attached'] = array(
    'js' => array(drupal_get_path('module', 'bibdk_openuserstatus') . '/js/getOpenUserStatus.js'),
  );
  $ret['bibdk_openuserstatus']['rows']['values'] = bibdk_openuser_status_my_page_get_rows();
  $ret['bibdk_openuserstatus']['rows']['attributes'] = array('class' => array('bibdk-openuserstatus-mypage-info'));
  $ret['bibdk_openuserstatus']['link'] = array(
    'title' => t('Go to userstatus'),
    'href' => 'user/' . $user->uid . '/edit/bibdk_openuserstatus',
  );

  return $ret;
}

function bibdk_openuser_status_my_page_get_rows() {
  $ret = array();
  $ret[] = array(
    'item_row' => t('retrieving_loan_status'),
  );
  
  return $ret;
}

?>
