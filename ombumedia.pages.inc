<?php

/**
 * Page callback for Manage Media page.
 */
function ombumedia_page_magage_media() {
  $view = views_get_view('ombumedia');
  $view->set_display('manage_media');
  $view->pre_execute();
  $view->execute();
  $view_rendered = $view->preview();

  drupal_add_library('ombumedia', 'ombumedia');

  return $view_rendered;
}

/**
 * Page callback for Select Media page.
 */
function ombumedia_page_select_media() {
  $view = views_get_view('ombumedia');
  $view->set_display('select_media');
  $view->pre_execute();
  $view->execute();

  $library = $view->preview();

  $upload = drupal_render(drupal_get_form('file_entity_add_upload'));

  $web = "WEB";

  drupal_add_library('ombumedia', 'ombumedia');

  return theme('ombumedia_select', array(
    'library' => $library,
    'upload' => $upload,
    'web' => $web,
  ));
}

/**
 * Page callback for Manage Media Ajax Upload page.
 */
function ombumedia_page_magage_media_ajax_upload() {
  module_load_include('inc', 'file_entity', 'file_entity.pages');
  $errors = array();
  if ($file = file_save_upload(0)) {

    $type_candidates = file_entity_get_filetype_candidates($file);

    if (count($type_candidates) >= 1) {

        if ($moved_file = file_move($file, 'public://' . file_uri_target($file->uri), FILE_EXISTS_RENAME)) {
          $type_candidates_keys = array_keys($type_candidates);
          $file = $moved_file;
          $file->type = reset($type_candidates_keys);;
        }
        else {
          $errors[] = 'Error saving file.';
        }

    }
    else {
      $errors[] = 'Unrecognized file type.';
    }

  }
  else {
    $errors[] = 'Invalid request';
  }

  if (empty($errors)) {
    $file->display = TRUE;
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);
    $file = file_load($file->fid);
    drupal_json_output(array(
      'file' => (array) $file,
    ));

  }
  else {
    drupal_json_output(array('errors' => $errors));
  }
  drupal_exit();
}

/**
 * Page callback to preview an OMBU Media file.
 */
function ombumedia_page_file_preview($file) {
  if (!user_access('administer files') && (file_uri_scheme($file->uri) === 'private')) {
    return MENU_ACCESS_DENIED;
  }

  drupal_add_library('ombumedia', 'ombumedia');

  return theme('ombumedia_file_preview', array(
    'file' => $file,
  ));
}

/**
 * Page callback to configure the selected file.
 */
function ombumedia_page_file_select($file) {
  if (!user_access('administer files') && (file_uri_scheme($file->uri) === 'private')) {
    return MENU_ACCESS_DENIED;
  }

  drupal_add_library('ombumedia', 'ombumedia');

  $link = '<a href="#" class="select-link" data-fid="' . $file->fid . '">Use file: ' . $file->filename . '</a>';
  return $link;
}

/**
 * Page callback to test popping up a media modal.
 */
function ombumedia_page_test() {
  $path = drupal_get_path('module', 'ombumedia');
  drupal_add_library('ombumedia', 'ombumedia');
  drupal_add_js($path . '/js/ombumedia-test.js');

  return <<<HEREDOC
<a href="#" class="select-media">Select Media</a>
<dl>
  <dt>FID</dt>
  <dd class="fid"></dd>
  <dt>View Mode</dt>
  <dd class="view-mode"></dd>
</dl>
HEREDOC;
}