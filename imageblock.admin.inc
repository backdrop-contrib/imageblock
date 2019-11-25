<?php

/**
 * @file
 * Functions regarding administration of Image blocks.
 */

/**
 * Form handler for the site wide Image block admin settings.
 */
function imageblock_admin_settings_form() {
  $config = config('imageblock.settings');
  $form['#config'] = 'imageblock.settings';
  $form['imageblock_image_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Image block image directory'),
    '#description' => t('Subdirectory in the file upload directory where images will be stored.'),
    '#default_value' => $config->get('imageblock_image_path'),
    '#maxlength' => 255,
  );
  $form['imageblock_max_file_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum file size'),
    '#description' => t('Specify the size limit that applies to each image. Enter a value like "512" (bytes), "80K" (kilobytes) or "50M" (megabytes) in order to restrict the allowed file size. If you leave this empty the file sizes will be limited only by PHP\'s maximum post and file upload sizes (current limit <strong>%limit</strong>).', array('%limit' => format_size(file_upload_max_size()))),
    '#default_value' => $config->get('imageblock_max_file_size'),
    '#maxlength' => 15,
  );
  $form['imageblock_max_dimensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum dimensions'),
    '#description' => t('The maximum allowed image size expressed as WIDTHxHEIGHT (e.g. 640x480). Set to 0 for no restriction. If a larger image is uploaded, it will be resized to reflect the given width and height.'),
    '#default_value' => $config->get('imageblock_max_dimensions'),
    '#maxlength' => 15,
  );
  return system_settings_form($form);
}

/**
 * Validation handler for the site wide Image block admin settings.
 */
function imageblock_admin_settings_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  if (!preg_match('/^[a-zA-Z0-9_-]*$/', $values['imageblock_image_path'])) {
    form_set_error('imageblock_image_path', t('The directory name contains invalid characters. Only alphanumeric characters, hyphens and underscores are allowed.'));
  }
  foreach (array('imageblock_max_file_size') as $size) {
    if (!empty($values[$size]) && !is_numeric(parse_size($values[$size]))) {
      form_error($size, t('The "@field" option must contain a valid value. You can either leave the text field empty or enter a string like "512" (bytes), "80K" (kilobytes) or "50M" (megabytes).', array('@field' => t('Maximum upload size per file'))));
    }
  }
  foreach (array('imageblock_max_dimensions') as $resolution) {
    if (!empty($values[$resolution]) && !preg_match('/^[0-9]+x[0-9]+$/', $values[$resolution])) {
      form_set_error($resolution, t('Please specify a resolution in the format WIDTHxHEIGHT (e.g. 640x480).'));
    }
  }
}

/**
 * Form handler to delete an Image block.
 */
function imageblock_block_delete($form, &$form_state, $machine_name) {
  $imageblock = config_get('imageblock.block.' . $machine_name);
  $form['machine_name'] = array(
    '#type' => 'hidden',
    '#value' => $imageblock['machine_name'],
  );
  $form['image'] = array(
    '#type' => 'hidden',
    '#value' => $imageblock['image'],
  );
  $confirm_message = t('Are you sure you want to delete the block %name?', array('%name' => $imageblock['label']));
  return confirm_form($form, $confirm_message, 'admin/structure/imageblock/list', '', t('Delete'), t('Cancel'));
}

/**
 * Form submission handler to delete an Image block.
 */
function imageblock_block_delete_submit($form, &$form_state) {
  if (!empty($form_state['values']['image'])) {
    $file = file_load($form_state['values']['image']);
    if ($file) {
      file_usage_delete($file, 'imageblock');
    }
  }
  config('imageblock.block.' . $form_state['values']['machine_name'])->delete();
  backdrop_set_message(t('The Image block %name has been removed.', array('%name' => $form_state['values']['machine_name'])));
  $form_state['redirect'] = 'admin/structure/imageblock/list';
}

/**
 * Callback to provide a listing page with all image blocks.
 */
function imageblock_list() {
  $header = array(
    t('Admin label'),
    t('Image'),
    t('Text preview'),
    t('Required for'),
    t('Action'),
  );
  $rows = array();
  $config_names = config_get_names_with_prefix('imageblock.block.');
  $imageblock_usage = imageblock_layout_usage();
  $parents = array();
  foreach ($config_names as $config_name) {
    $parent = config_get($config_name, 'parent');
    if ($parent && $parent != 'default') {
      $parents[] = $parent;
    }
  }

  foreach ($config_names as $config_name) {
    $block = config_get($config_name);
    $preview_text = t('No text set');
    if (!empty($block['title'])) {
      $plain = strip_tags($block['title']);
      $preview_text = $plain;
    }
    elseif (!empty($block['content'])) {
      $plain = strip_tags($block['content']);
      $preview_text = substr($plain, 0, 30);
    }
    $image = '-';
    if (!empty($block['image'])) {
      $file = file_load($block['image']);
      if ($file) {
        $variables = array(
          'style_name' => 'thumbnail',
          'uri' => $file->uri,
          'width' => isset($file->width) ? $file->width : NULL,
          'height' => isset($file->height) ? $file->height : NULL,
          'alt' => '',
          'attributes' => array(),
        );
        $image = theme('image_style', $variables);
      }
    }
    $required_for = '-';
    if (array_key_exists($block['machine_name'], $imageblock_usage)) {
      $current_usage = $imageblock_usage[$block['machine_name']];
      $admin_path = 'admin/structure/layouts/manage/' . $current_usage['layout'];
      $options = array(
        'fragment' => 'layout-editor-region-' . $current_usage['region'],
      );
      $linktext = $current_usage['layout_title'] . ' / ' . $current_usage['region'];
      $required_for = l($linktext, $admin_path, $options);
    }
    $action = '-';
    if ($required_for == '-') {
      if (!in_array($block['machine_name'], $parents)) {
        $path = 'admin/structure/imageblock/' . $block['machine_name'] . '/delete';
        $link_options = array('attributes' => array('class' => array('button')));
        $action = l(t('Delete'), $path, $link_options);
      }
      else {
        $required_for = t('Has dependent block');
      }
    }
    $rows[] = array(
      $block['label'],
      $image,
      $preview_text,
      $required_for,
      $action,
    );
  }
  $markup = '<p>' . t('Please note: you can only delete Image blocks not in use in any layout and not parent of a copied block in use.') . '</p>';
  $markup .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No image blocks (yet).'),
  ));
  return $markup;
}

/**
 * Access callback function for imageblock delete form.
 */
function imageblock_delete_access($machine_name) {
  $block = config_get('imageblock.block.' . $machine_name);
  if ($block && user_access('administer layouts')) {
    $config_names = config_get_names_with_prefix('imageblock.block.');
    $imageblock_usage = imageblock_layout_usage();
    $parents = array();
    foreach ($config_names as $config_name) {
      $parent = config_get($config_name, 'parent');
      if ($parent && $parent != 'default') {
        $parents[] = $parent;
      }
    }
    if (in_array($block['machine_name'], $parents)) {
      // Image block needed by a copy block.
      return FALSE;
    }
    if (array_key_exists($block['machine_name'], $imageblock_usage)) {
      // Image block is in use in a layout.
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

/**
 * Helper function to get all Image block usages in layouts.
 */
function imageblock_layout_usage() {
  $imageblocks_in_use = array();
  module_load_include('inc', 'layout', 'layout.admin');
  @$usage = layout_get_block_usage();
  if (isset($usage['imageblock'])) {
    $delta = $usage['imageblock'];
    foreach ($delta as $layouts) {
      foreach ($layouts as $layout_name => $layout) {
        foreach ($layout as $region_name => $region) {
          foreach ($region as $imageblock) {
            $imageblocks_in_use[$imageblock->settings['machine_name']] = array(
              'layout' => $layout_name,
              'layout_title' => $imageblock->layout_title,
              'region' => $region_name,
            );
          }
        }
      }
    }
  }
  return $imageblocks_in_use;
}
