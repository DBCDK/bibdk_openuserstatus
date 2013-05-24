<?php dpm($variables); ?>
<!-- table actions -->
<div class="table-actions">
  <button class="markall-button">
    <input type="checkbox" value="0">Markér alle lån
  </button>
  <?php foreach ($actions as $action) : ?>
    <?php print drupal_render($action); ?>
  <?php endforeach; ?>
  <!--
  <button class="delete-button" href="#">
    <span class="icon icon-left icon-green-refresh">▼</span>Genlån
  </button> -->
</div>
<!-- table actions -->