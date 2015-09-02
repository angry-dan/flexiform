<?php
/**
 * @file
 * Contains class for the User mail element.
 */

/**
 * Class to add the User mail field to a form.
 */
class FlexiformElementUserMail extends FlexiformElement {

  /**
   * Return the form element for this FlexiformElement.
   */
  public function form($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $parents = $form['#parents'];
    $parents[] = 'mail';

    // Work out the default value.
    $default = '';
    if (!empty($this->settings['default_value']['default_value'])) {
      $default = $this->settings['default_value']['default_value'];
    }
    if (!empty($this->settings['default_value']['use_tokens'])) {
      $default = $this->replaceCtoolsSubstitutions($default, $form['#flexiform_entities']);
    }

    $form[$this->element_namespace] = array(
      '#type' => 'textfield',
      '#parents' => $parents,
      '#title' => $this->label(),
      '#required' => TRUE,
      '#default_value' => !empty($entity->mail) ? $entity->mail : $default,
      '#maxlength' => EMAIL_MAX_LENGTH,
    );

    $form = parent::form($form, $form_state, $entity);

    return $form;
  }

  /**
   * Validate the form element.
   */
  public function formValidate($form, &$form_state, $entity, $language = LANGUAGE_NONE) {

    // Trim whitespace from mail, to prevent confusing 'e-mail not valid'
    // warnings often caused by cutting and pasting.
    $mail = trim($this->formExtractValues($form, $form_state, $entity));
    $this->formSetValue($mail, $form, $form_state);

    $uid = empty($entity->is_new) ? $entity->uid : 0;

    // Validate the e-mail address, and check if it is taken by an existing
    // user.
    if ($error = user_validate_mail($mail)) {
      form_set_error($this->getElementNamespace(), $error);
    }
    elseif ((bool) db_select('users')
      ->fields('users', array('uid'))
      ->condition('uid', $uid, '<>')
      ->condition('mail', db_like($mail), 'LIKE')
      ->range(0, 1)
      ->execute()
      ->fetchField()) {

      form_set_error($this->getElementNamespace(), t('The e-mail address %email is already registered.', array('%email' => $mail)));
    }
  }

  /**
   * Submit the form element.
   */
  public function formSubmit($form, &$form_state, $entity, $language = LANGUAGE_NONE) {
    $mail = $this->formExtractValues($form, $form_state, $entity);
    $entity->mail = $mail;
  }

  /**
   * Extract the submitted values for this form element.
   */
  public function formExtractValues($form, &$form_state, $entity) {
    $parents = $form[$this->getElementNamespace()]['#parents'];
    return drupal_array_get_nested_value($form_state['values'], $parents);
  }

  public function formSetValue($value, $form, &$form_state) {
    form_set_value($form[$this->getElementNamespace()], $value, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function configureForm($form, &$form_state, $flexiform) {
    $form = parent::configureForm($form, $form_state, $flexiform);

    $form['default_value'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('Default Value'),
      '#weight' => -5,
    );

    $form['default_value']['default_value'] = array(
      '#type' => 'textfield',
      '#title' => $this->label() ? $this->label() : t('Default'),
      '#default_value' => !empty($this->settings['default_value']['default_value']) ? $this->settings['default_value']['default_value'] : '',
      '#maxlength' => 255,
    );

    $form['default_value']['use_tokens'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use Tokens in Default Value'),
      '#default_value' => !empty($this->settings['default_value']['use_tokens']),
    );

    $form['default_value']['contexts'] = array(
      '#title' => t('Substitutions'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['default_value']['contexts']['contexts'] = $this->getCtoolsSubstitutionsList();

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function configureFormSubmit($form, &$form_state, $flexiform) {
    $this->settings['default_value']['default_value'] = $form_state['values']['default_value']['default_value'];
    $this->settings['default_value']['use_tokens'] = $form_state['values']['default_value']['use_tokens'];

    parent::configureFormSubmit($form, $form_state, $flexiform);
  }

  /**
   * {@inheritdoc}
   */
  public function toSettingsArray() {
    $settings = parent::toSettingsArray();

    if (isset($this->settings['default_value'])) {
      $settings['default_value'] = $this->settings['default_value'];
    }

    return $settings;
  }
}
