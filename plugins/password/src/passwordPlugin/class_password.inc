<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_password.inc 20453 2010-11-30 16:56:44Z cajus $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class password extends plugin
{
    /* Definitions */
    var $plHeadline     = "Change password";
    var $plDescription  = "Change user password";

    var $proposal = "";
    var $proposalEnabled = FALSE;
    var $proposalSelected = FALSE;
    var $proposalInitialized = FALSE;

    var $forcedHash = NULL;

    function password(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::plugin($config, $dn, $parent);
    }

    
    function forceHash($hash)
    {
        $this->forcedHash = $hash;
    }    


    function refreshProposal()
    {
        $this->proposal = passwordMethod::getPasswordProposal($this->config);
        $this->proposalEnabled = (!empty($this->proposal));
    }


    function execute()
    {
        plugin::execute();
        $smarty = get_smarty();
        $ui = get_userinfo();


        // Try to generate a password proposal, if this is successfull 
        //  then preselect the proposal usage. 
        if(!$this->proposalInitialized){
            $this->refreshProposal();
            if($this->proposal != ""){
                $this->proposalSelected = TRUE;
            }
            $this->proposalInitialized = TRUE;
        }

        /* Get acls */
        $password_ACLS = $ui->get_permissions($ui->dn,"users/password");
        $smarty->assign("ChangeACL" ,  $password_ACLS);
        $smarty->assign("NotAllowed" , !preg_match("/w/i",$password_ACLS));

        /* Display expiration template */
        $smarty->assign("passwordExpired", FALSE);
        if ($this->config->boolValueIsTrue("core","handleExpiredAccounts")){
            $expired= ldap_expired_account($this->config, $ui->dn, $ui->username);
            $smarty->assign("passwordExpired", $expired & POSIX_FORCE_PASSWORD_CHANGE);
            if($expired == POSIX_DISALLOW_PASSWORD_CHANGE){
                return($smarty->fetch(get_template_path("nochange.tpl", TRUE)));
            }
        }
        
        // Refresh proposal if requested
        if(isset($_POST['refreshProposal'])) $this->refreshProposal();
        if(isset($_POST['proposalSelected'])) $this->proposalSelected = get_post('proposalSelected') == 1;
        $smarty->assign("proposal" , set_post($this->proposal));
        $smarty->assign("proposalEnabled" , $this->proposalEnabled);
        $smarty->assign("proposalSelected" , $this->proposalSelected);

        /* Pwd change requested */
        if (isset($_POST['password_finish'])){
    
            if($this->proposalSelected){
                $current_password = get_post('current_password');
                $new_password = $this->proposal;
                $repeated_password = $this->proposal;
            }else{
                $current_password = get_post('current_password');
                $new_password = get_post('new_password');
                $repeated_password = get_post('repeated_password');
            }


            // Get configuration flags for further input checks.
            $check_differ = $this->config->get_cfg_value("core","passwordMinDiffer") != "";
            $differ       = $this->config->get_cfg_value("core","passwordMinDiffer");
            $check_length = $this->config->get_cfg_value("core","passwordMinLength") != "";
            $length       = $this->config->get_cfg_value("core","passwordMinLength");

            // Once an error has occured it is stored here.
            $message = array();

            // Call the check hook
            $attrs = array();
            $attrs['current_password'] = ($current_password);
            $attrs['new_password'] = ($new_password);

            // Perform GOsa password policy checks 
            if(empty($current_password)){
                $message[] = tr("You need to specify your current password in order to proceed.");
            }elseif($new_password  != $repeated_password){
                $message[] = tr("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
            }elseif($new_password == ""){
                $message[] = tr("The password you've entered as 'New password' is empty.");
            }elseif($check_differ && (substr($current_password, 0, $differ) == substr($new_password, 0, $differ))){
                $message[] = tr("The password used as new and current are too similar.");
            }elseif($check_length && (strlen($new_password) < $length)){
                $message[] = tr("The password used as new is to short.");
            }elseif(!passwordMethod::is_harmless($new_password)){
                $message[] = tr("The password contains possibly problematic Unicode characters!");
            }

            // Call external check hook to validate the password change
            if(!count($message)){
                $checkRes = $this->callCheckHook($this->config,$this->dn,$attrs);
                if(count($checkRes)){
                    $message[] = sprintf(tr("Check-hook reported a problem: %s. Password change canceled!"),implode($checkRes));
                }
            }

            // Some errors/warning occured, display them and abort password change.
            if(count($message)){
                msg_dialog::displayChecks($message);
            }else{ 

                /* Try to connect via current password */
                $tldap = new LDAP(
                        $ui->dn, 
                        $current_password,
                        $this->config->current['SERVER'],
                        $this->config->get_cfg_value("core","ldapFollowReferrals") == "true",
                        $this->config->get_cfg_value("core","ldapTLS") == "true");

                /* connection Successfull ? */
                if (!$tldap->success()){
                    msg_dialog::display(tr("Password change"),
                            tr("The password you've entered as your current password doesn't match the real one."),WARNING_DIALOG);
                }elseif (!preg_match("/w/i",$password_ACLS)){
                    msg_dialog::display(tr("Password change"),
                            tr("You have no permission to change your password."),WARNING_DIALOG);
                }elseif(!change_password($ui->dn, $new_password,FALSE, $this->forcedHash, $current_password, $message)){
                    msg_dialog::display(tr("Password change"),
                            $message,WARNING_DIALOG);
                }else{
                    $ui->password= $new_password;
                    session::set('ui',$ui);
                    return($smarty->fetch(get_template_path("changed.tpl", TRUE)));
                }
            }
        }
        return($smarty->fetch(get_template_path("password.tpl", TRUE)));
    } 

    function remove_from_parent()
    {
        $this->handle_post_events("remove");
    }

    function save()
    {
    }

    static function callCheckHook($config,$dn,$attrs = array())
    {
        $command = $config->configRegistry->getPropertyValue("password","check");
        if (!empty($command)){

            // Build up ldif to send to the check hook
            $ldif= "dn: $dn\n";
            foreach ($attrs as $name => $value){
                $ldif.= "{$name}: {$value}\n";
            }
            $ldif.= "\n";

            /* Feed "ldif" into hook and retrieve result*/
            $descriptorspec = array( 0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));
            $fh= proc_open($command, $descriptorspec, $pipes);
            if (is_resource($fh)) {
                fwrite ($pipes[0], $ldif);
                fclose($pipes[0]);
                $arr= stream_get_contents($pipes[1]);
                $err = stream_get_contents($pipes[2]);
                fclose($pipes[1]);
                fclose($pipes[2]);
                $returnCode = proc_close($fh);

                if($returnCode !== 0){
                    @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execution failed code: ".$returnCode);
                    @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Result: ".$err);
                    $message= msgPool::cmdexecfailed($err,$command, "password");
                    msg_dialog::display(tr("Error"), $message, ERROR_DIALOG);
                }else{
                    @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Result: ".$arr);
                    return(preg_split("/\n/", $arr,0,PREG_SPLIT_NO_EMPTY));
                }
            }
        }
        return(array());
    }

    static function plInfo()
    {
        return (array(
                    "plDescription" => tr("User password"),
                    "plSelfModify"  => TRUE,
                    "plDepends"     => array("user"),
                    "plPriority"    => 10,
                    "plSection"     => array("personal" => tr("My account")),
                    "plCategory"    => array("users"),
                    "plOptions"     => array(),

                    "plProperties" => array(
                        array(
                            "name"          => "PRELOCK",
                            "type"          => "command",
                            "default"       => "",
                            "description"   => tr("Script to be called before a password gets locked."),
                            "check"         => "gosaProperty::isCommand",
                            "migrate"       => "",
                            "group"         => "password",
                            "mandatory"     => FALSE),
                        array(
                            "name"          => "POSTLOCK",
                            "type"          => "command",
                            "default"       => "",
                            "description"   => tr("Script to be called after a password gets locked."),
                            "check"         => "gosaProperty::isCommand",
                            "migrate"       => "",
                            "group"         => "password",
                            "mandatory"     => FALSE),
                        array(
                            "name"          => "PREUNLOCK",
                            "type"          => "command",
                            "default"       => "",
                            "description"   => tr("Script to be called before a password gets unlocked."),
                            "check"         => "gosaProperty::isCommand",
                            "migrate"       => "",
                            "group"         => "password",
                            "mandatory"     => FALSE),
                        array(
                            "name"          => "POSTUNLOCK",
                            "type"          => "command",
                            "default"       => "",
                            "description"   => tr("Script to be called after a password gets unlocked."),
                            "check"         => "gosaProperty::isCommand",
                            "migrate"       => "",
                            "group"         => "password",
                            "mandatory"     => FALSE)
                        ),
                    "plProvidedAcls"  => array())
               );
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
