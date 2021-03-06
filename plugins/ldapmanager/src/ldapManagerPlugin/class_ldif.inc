<?php

class ldif extends plugin
{
    /* Definitions */
    var $plHeadline= "LDAP tools";
    var $plDescription= "Export LDIF or Excel snapshots of the LDAP tree";
    var $plIcon = "plugins/ldapmanager/html/images/ldif.png";

    /* attribute list for save action */
    var $attributes= array();
    var $objectclasses= array();
    var $ldif;

    function ldif (&$config)
    {
        $this->initTime = microtime(TRUE);

        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));


        /* Include config object */
        $this->ldif= new ldiftab($config, $config->data['TABS']['LDIFTAB'], "");

    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Show main page */
        return ($this->ldif->execute());
    }

    /* Return plugin informations for acl handling
#FIXME You can only read attributes within this report plugin */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => tr("LDAP manager"),
                    "plDescription" => tr("LDAP manager")."&nbsp;(".tr("import")."/".tr("export").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("addon"),
                    "plCategory"    => array("ldapmanager" => array("description" => tr("LDAP manager"))),

                    "plProvidedAcls" => array()
                    ));
    }



}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
