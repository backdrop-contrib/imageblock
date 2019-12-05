<?php

/**
 * @file
 * Default theme implementation to display image block content.
 *
 * Available variables:
 * - $image: Block image.
 * - $img_url: Image url (for background-image).
 * - $content: Block content.
 * - $wrapper_classes: Block content wrapper class(es).
 *
 * @see template_preprocess_block()
 */
?>
<?php if ($img_url): ?>
<div class="<?php print $wrapper_classes;?>" style="background-image: url(<?php print $img_url ?>)">
<?php else: ?>
<div class="<?php print $wrapper_classes;?>">
<?php endif; ?>
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
