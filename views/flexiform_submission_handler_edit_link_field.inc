<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying edit links
 * as fields
 */


class flexiform_submission_handler_edit_link_field extends flexiform_submission_handler_link_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
  }


  function render($values) {
    $type = $values->{$this->aliases['type']};
    
    //Creating a dummy flexiform_submission to check access against
    $dummy_flexiform_submission = (object) array('type' => $type);
    if (!flexiform_submission_access('edit', $dummy_flexiform_submission)) {
      return;
    }
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $flexiform_submission_id = $values->{$this->aliases['flexiform_submission_id']};
    
    return l($text, 'admin/content/flexiform_submissions/flexiform_submission/' . $flexiform_submission_id . '/edit');
  }
}