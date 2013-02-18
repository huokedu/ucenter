<?php

/**
 * * get {ucenter_users} drupal uid
 * *
 * * @return void
 * **/
function ucenter_getdrupaluid ($ucenter_uid){
  $result = db_result(db_query('SELECT * FROM {ucenter_users}  WHERE ucenter_uid = %d ', (int)$ucenter_uid));
  if (!$result){
    return FALSE;
  }  
  return $result->drupal_uid;
}



/**
 * * load {ucenter_users}
 * *
 * * @return void
 * **/
function ucenter_load ($ucenter_info = array()) {
  $query = array();
  $params = array();

  if (is_numeric($ucenter_info)) {
    $ucenter_info = array('drupal_uid' => $ucenter_info);
  }
  elseif (!is_array($ucenter_info)) {
    return FALSE;
  }
  
  foreach ($ucenter_info as $key => $value) {
    if ($key == 'drupal_uid' || $key == 'ucenter_uid' || $key == 'drupal_actived') {
      $query[] = "$key = %d";
      $params[] = $value;
    }
    else {
      $query[] = "LOWER($key) = LOWER('%s')";
      $params[] = $value;
    }
  }
  $result = db_query('SELECT * FROM {ucenter_users} ucenter WHERE ' . implode(' AND ', $query), $params);

  if ($ucenter_user = db_fetch_object($result)) {
    $ucenter_user = drupal_unpack($ucenter_user);
  }
  else {
    $ucenter_user = FALSE;
  }

  return $ucenter_user;
}

/**
 * * delete {ucenter_users} record
 * *
 * * @return void
 * **/
function ucenter_delete ($drupal_uid)  {
  return db_query('DELETE FROM {ucenter_users} WHERE drupal_uid = %d ', $drupal_uid);
}

/**
 * * insert or update {ucenter_users} record
 * *
 * * @return void
 * **/
function ucenter_save($ucenter_account, $array = array()){
  if (is_object($ucenter_account) && $ucenter_account->drupal_uid) {
    $query = '';
    foreach ($array as $key => $value) {
      if (!empty($key)) {
        if ($key == 'drupal_uid' || $key == 'ucenter_uid' || $key == 'drupal_actived') {
          $query .= "$key = %d , ";
          $v[] = $value;
        }
      }    
    }
    $success = db_query("UPDATE {ucenter_users} SET $query WHERE ucenter_uid = %d", array_merge($v, array($ucenter_account->drupal_uid)));
    if (!$success) {
      return FALSE;
    }
  }
  else{
    foreach ($array as $key => $value) {
      if ($key == 'drupal_uid' || $key == 'ucenter_uid' || $key == 'drupal_actived') {
        $fields[] = $key;
        $values[] = $value;
        $s[] = "%d";
      }
    }
    $success = db_query('INSERT INTO {ucenter_users} (' . implode(', ', $fields) . ') VALUES (' . implode(', ', $s) . ')', $values);
    if (!$success) {
      return FALSE;
    }
  }
  return TRUE;
}
