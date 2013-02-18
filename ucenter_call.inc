<?php 
/**
 * *  Delete drupal user . called by Ucenter
 * *
 * * @return BOOLEAN
 * **/
function ucenter_deleteuser($str_ucenter_uids){
    $str_ucenter_uids = iconv(UC_CHARSET, 'UTF-8', $str_ucenter_uids);
    $str_ucenter_uids = str_replace("'", "", $str_ucenter_uids);
    $str_ucenter_uids = str_replace('"', '', $str_ucenter_uids);
    $ucenter_uids = explode(',',$str_ucenter_uids);
    foreach($ucenter_uids as $ucenter_uid){
        if (!empty($ucenter_uid)) {
            $ucenter_user = ucenter_load(array('ucenter_uid' => (int)$ucenter_uid,));
            if (!$ucenter_user){
                break;
            }      
            ucenter_delete($ucenter_user->drupal_uid);
            user_delete(array(), $ucenter_user->drupal_uid);
            watchdog('user', 'Ucenter: Delete user(drupal uid:%uid) by Ucenter.', array('%uid' => $ucenter_user->drupal_uid));
        }
    }
    return TRUE;
}

/**
 * * rename drupal user .called by Ucenter
 * *
 * * @return void
 * **/
function ucenter_renameuser ($ucenter_uid, $usernameold, $usernamenew) {
    $usernameold = iconv(UC_CHARSET, 'UTF-8', $usernameold);
    $usernamenew = iconv(UC_CHARSET, 'UTF-8', $usernamenew);

    $ucenter_user = ucenter_load(array('ucenter_uid' => (int) $ucenter_uid,));

    if (!$ucenter_user){
        return FALSE;
    }  
    $account= user_load(array('uid' => $ucenter_user->drupal_uid, ));  
    if (!$account){
        return FALSE;
    }

    if (!db_query(" UPDATE {users} SET name='%s' WHERE uid=%d ", $usernamenew, $account->uid)) {
        return FALSE;
    }
    watchdog('user', 'Ucenter: Changed use name. UID:%drupal_uid  OldUerName: %usernameold NewUserName: %usernamenew ', array('%drupal_uid' => $ucenter_uid, '%usernameold' => $usernameold,'%usernamenew' => $usernamenew,) );
    return TRUE;

}

/**
 * * synlogin . 
 * * When user login from ucenter ,find user in drupal. 
 * * If found ,login drupal else create it in drupal.
 * * 
 * *
 * * @return void
 * **/
function ucenter_synlogin ($ucenter_uid,$username) {
    $username = iconv(UC_CHARSET, 'UTF-8', $username);
    global $user;
    $account= user_load(array('name' => $username, )); 
    $ucenter_user = ucenter_load(array('ucenter_uid' => (int) $ucenter_uid,));

    if (!$account) {
        watchdog('user', 'Ucenter: User name (%name)  , 不存在drupal中，無法登陸.', array('%name' => $username));
        return FALSE;
    }
    else {
        if (!$ucenter_user ){
            watchdog('user', 'Ucenter: User name (%name)  , 不存在drupal與ucenter的關系表中，無法登陸.', array('%name' => $username));
            return FALSE;
        }
    }

    // 下面三行更新登陸的狀態信息
    watchdog('user', 'Session opened for %name.', array('%name' => $user->name));
    $user = $account;
    $user->login = time();
    db_query("UPDATE {users} SET login = %d WHERE uid = %d", $user->login, $user->uid);
    // 消毀請求產生的匿名的會話id,重新生會話id,在生命周期請求結束后，sess_write()
    // 會把會話信息寫到數據庫里面去的,以便下次請求識別會話。下面那個函數會生成以cookie形式的會話id.
    sess_regenerate();
   // user_module_invoke('login', $edit, $user);
    

    //    user_external_login_register($username, 'ucenter');

    if (!$ucenter_user) {
        ucenter_save('',array('drupal_uid' => $user->uid, 'ucenter_uid' => (int) $ucenter_uid, 'drupal_actived' => 0,));
    }
    watchdog('user', 'Ucenter: Login successful for %name.', array('%name' => $username));
    return TRUE;

}


/**
 * *  synlogout . called by Ucenter.
 * *
 * * @return boolean
 * **/
function ucenter_synlogout () {
    global $user;
    $ucenter_user = ucenter_load($user->uid);      
    if (!$ucenter_user){
        return FALSE;
    }
     //刪除數據表session中的數據。
    session_destroy();
    //刪除$_SESSION中的數據。
    session_unset();
    // 清了瀏覽器中的會話cookie
    setcookie(session_name(), '', time() - 42000, '/');
    
    $user = drupal_anonymous_user();
    watchdog('user', 'Ucenter: Session closed for %name.', array('%name' => $user->name));
    return TRUE;
}


/**
 * * update pass . called by Ucenter.
 * * @return boolean
 * * notice: Ucenter did not send updatepw messages untill drupal update user pass 
 * **/
function ucenter_updatepw($username,$password) {
    $username = iconv(UC_CHARSET, 'UTF-8', $username);
    $password = iconv(UC_CHARSET, 'UTF-8', $password);

    if (empty($username) || empty($password)) {
        watchdog('user', 'Ucenter: receive empty username (%username) or password (%password).', array('%username' => $username, '%password' => $password ), WATCHDOG_WARNING );
        return TRUE;
    }

    $account= user_load(array('name' => $username, )); 
    if (!$account){
        return FALSE;
    }
    $ucenter_user = ucenter_load(array('drupal_uid' => $account -> uid,));
    if (!$ucenter_user){
        return FALSE;
    }
    if (!db_query(" UPDATE {users} SET pass=md5('%s') WHERE name='%s' ", $password, $username)) {
        return FALSE;
    }
    watchdog('user', 'Ucenter: Changed use pass. username:%username  ', array('%username' => $username, ) ); 
    return TRUE;
}
