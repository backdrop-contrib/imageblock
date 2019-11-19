<?php

/**
 * @file
 * Default theme implementation to display image block content.
 *
 * Available variables:
 * - $image: Block image.
 * - $content: Block content.
 * - $wrapper_classes: Block content wrapper class(es).
 *
 * @see template_preprocess_block()
 */
?>
<div class="<?php print $wrapper_classes;?>">
<?php if ($image): ?>
  <div class="block-image">
    <?php print $image ?>
  </div>
<?php endif; ?>
<?php if ($content): ?>
  <div class="block-body">
    <?php print $content ?>
  </div>
<?php endif; ?>
</div>
