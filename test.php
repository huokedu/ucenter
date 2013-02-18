<?php 
    require_once dirname(__FILE__).'/config.inc.php';
    require_once dirname(__FILE__).'/uc_client/client.php';
    $edit = array(
            'name' => ' ',
            'pass' => ' ',
    );
    @list($ucenter_uid, $ucenter_username, $ucenter_password, $ucenter_email) = uc_user_login($edit['name'], $edit['pass'],0);
    if ($ucenter_uid > 0){ 
      @$ucsyninfo = uc_user_synlogin($ucenter_uid);
      echo $ucsyninfo;
    }
?>
