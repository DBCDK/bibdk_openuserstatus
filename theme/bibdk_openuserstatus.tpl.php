<div class="userstatus">
  <h5>Loanerstatus</h5>

  <p>blah-blah</p>
  <p>Go to:</p>
  <a href="#loans">loans</a>
  <a href="#reservations">reservations</a>
  <a href="#fiscal">fiscal</a>
  <hr/>
  <!-- LOANS -->
  <a name="loans"></a>
  <h5><?php print t('your_loans %count', array('%count' => $loans_count), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print drupal_render($loans); ?>
  <!-- LOANS -->

  <!-- READY FOR PICKUP -->
  <a name="reservations"></a>
  <h4><?php print t('your_reservations', array(), array('context' => 'bibdk_openuserstatus')); ?></h4>

  <h5><?php print t('reservations_ready_for_pickup', array(), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print drupal_render($ready_for_pickup); ?>
  <!-- READY FOR PICKUP -->

  <!-- RESERVATIONS -->
  <h5><?php print t('reservations_in_process', array(), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print drupal_render($reservations); ?>
  <!-- RESERVATIONS -->

  <!-- DEBT -->
  <a name="fiscal"></a>
  <h5><?php print t('fiscal_account', array(), array('context' => 'bibdk_openuserstatus')); ?></h5>
  <?php print drupal_render($fiscal_account); ?>
  <!-- DEBT -->
</div>