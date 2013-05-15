<div class="userstatus">
  <h5>Loanerstatus</h5>

  <p>blah-blah</p>
  <hr/>
  <!-- LOANS -->
  <h5><?php print t('your_loans %count', array('%count' => $loans_count), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print drupal_render($loans); ?>
  <!-- LOANS -->

  <!-- READY FOR PICKUP -->
  <h4><?php print t('your_reservations', array(), array('context' => 'bibdk_openuserstatus')); ?></h4>

  <h5><?php print t('ready_for_pickup', array(), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print drupal_render($ready_for_pickup); ?>
  <!-- READY FOR PICKUP -->

  <!-- RESERVATIONS -->
  <h5><?php print t('reservations', array(), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print drupal_render($reservations); ?>
  <!-- RESERVATIONS -->

  <!-- DEBT -->
  <h5><?php print t('fiscal_account', array(), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print $fiscal_account; ?>
  <!-- DEBT -->
</div>