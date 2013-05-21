<section>
  <a name="loans"></a>

  <div class="element-wrapper">
    <div class="element">
      <div class="element-section padded">
        <div class="table-actions">
          <button class="markall-button">
            <input type="checkbox" value="0">Markér alle lån
          </button>
          <button class="delete-button" href="#">
            <span class="icon icon-left icon-green-refresh">▼</span>Genlån
          </button>
        </div>
        <!-- table actions -->
        <div class="element-title">
          <h2><?php print t('your_loans %count', array('%count' => $count), array('context' => 'bibdk_openuserstatus')); ?></h2>
        </div>
      </div>
      <!-- element-section -->
      <div class="element-section">
        <div class="table">
          <?php print drupal_render($table); ?>
        </div>
      </div>
    </div>
  </div>
</section>