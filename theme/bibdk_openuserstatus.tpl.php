<div class="userstatus">
  <h2>Loanerstatus</h2>

  <p>blah-blah</p>

  <p>Go to:</p>
  <a href="#loans">loans</a>
  <a href="#reservations">reservations</a>
  <a href="#fiscal">fiscal</a>

  <?php foreach ($tables as $table) : ?>
    <?php print $table; ?>
  <?php endforeach; ?>
</div>