<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_groupManagement.inc 19290 2010-07-29 16:21:26Z cajus $$
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

class groupManagement extends management
{
    var $plHeadline     = "Groups";
    var $plDescription  = "Manage aspects of groups like members, POSIX, desktop, samba and mail settings";
    var $plIcon  = "plugins/groups/html/images/plugin.png";

    // Tab definition 
    protected $tabClass = "grouptabs";
    protected $tabType = "GROUPTABS";
    protected $aclCategory = "groups";
    protected $aclPlugin   = "group";
    protected $objectName   = "group";

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;

        $this->storagePoints = array(get_ou("core", "groupRDN"));

        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("group-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("group-list.xml", true));
        $headpage->registerElementFilter("filterProperties", "groupManagement::filterProperties");
        $headpage->setFilter($filter);

        // Add copy&paste and snapshot handler.
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            $this->cpHandler = new CopyPasteHandler($this->config);
        }
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }
        parent::__construct($config, $ui, "groups", $headpage);

        $this->registerAction("edit_group","editEntry");
        $this->registerAction("edit_group_","editEntry");
        $this->registerAction("edit_group__","editEntry");
        $this->registerAction("edit_environment","editEntry");
        $this->registerAction("edit_appgroup","editEntry");
        $this->registerAction("edit_mailgroup","editEntry");

        $this->registerAction("sendMessage", "sendMessage");
        $this->registerAction("cancel_notification", "closeDialogs");
        $this->registerAction("send_notification", "sendMessageNow");
    }


    // Inject user actions 
    function detectPostActions()
    {
        $action = management::detectPostActions();
        if(isset($_POST['cancel_notification'])) $action['action'] = "cancel_notification";
        if(isset($_POST['send_notification'])) $action['action'] = "send_notification";
        return($action);
    }


    /*! \brief      Finally sends a notification initiated by sendMessage().
     */   
    function sendMessageNow()
    {
        if($this->dialogObject instanceOf ngEventNotifyUser){
            $this->dialogObject->save_object();
            $msgs = $this->dialogObject->check();
            if(count($msgs)){
                msg_dialog::displayChecks($msgs);
            }else{
                if($this->dialogObject->send()){
                    $this->closeDialogs();
                }
            }
        }
    }


    /*! \brief      Opens the notification dialog to send a 
     *              message to the selected users.
     *              
     *              This methos is called when the notification 
     *              menu entry is clicked.
     */
    function sendMessage($action="",$target=array(),$all=array())
    {
        $uids = array();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        foreach($target as $dn){
            $ldap->cat($dn,array('memberUid'));
            $attrs = $ldap->fetch();
            if(isset($attrs['memberUid'])){
                unset($attrs['memberUid']['count']);
                $uids = array_merge($uids, $attrs['memberUid']);
            }
        }
        if(count($uids)){
            $this->dialogObject = new ngEventNotifyUser($this->config, NULL);
            $this->dialogObject->setRecipients($uids, array());
        }
    }


    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $str = management::editEntry($action,$target);
        if(preg_match("/^edit_/",$action)){
            $tab = preg_replace("/^edit_([^_]*).*$/","\\1",$action);
            if(isset($this->tabObject->by_object[$tab])){
                $this->tabObject->current = $tab;
            }else{
                trigger_error("Unknown tab: ".$tab);
            }
        }
        if(!empty($str)) return($str);
    }

    static function filterProperties($row, $classes)
    {
        $result= "";

        $map = array(
                "posixGroup" => 
                array(
                    "image" => "plugins/groups/html/images/select_group.png",
                    "plugin" => "group",
                    "alt" => tr("POSIX"),
                    "title" => tr("Edit POSIX properties")
                    ),

                "gosaMailAccount" => 
                array(
                    "image" => "plugins/groups/html/images/mail.png",
                    "plugin" => "mailgroup",
                    "alt" => tr("Mail"),
                    "title" => tr("Edit mail properties")
                    ),

                "sambaGroupMapping" => 
                array(
                    "image" => "plugins/groups/html/images/samba.png",
                    "plugin" => "group_",
                    "alt" => tr("Samba"),
                    "title" => tr("Edit samba properties")
                    ),

                "goFonPickupGroup" => 
                    array(
                            "image" => "plugins/groups/html/images/asterisk.png",
                            "plugin" => "group__",
                            "alt" => tr("Phone"),
                            "title" => tr("Edit phone properties")
                         ),

                "gotoMenuGroup" => 
                    array(
                            "image" => "plugins/groups/html/images/menu.png",
                            "plugin" => "appgroup",
                            "alt" => tr("Menu"),
                            "title" => tr("Edit start menu properties")
                         ),

                "gotoEnvironment" => 
                    array(
                            "image" => "plugins/groups/html/images/environment.png",
                            "plugin" => "environment",
                            "alt" => tr("Environment"),
                            "title" => tr("Edit environment properties")
                         )
                    );


        // Walk thru map
        foreach ($map as $oc => $properties) {
            if (in_array_ics($oc, $classes)) {
                $result.= image($properties['image'], 'listing_edit_'.$properties['plugin'].'_'.$row, $properties['title']);
            } else {
                $result.= image('images/empty.png');
            }
        }
        return $result;
    }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
