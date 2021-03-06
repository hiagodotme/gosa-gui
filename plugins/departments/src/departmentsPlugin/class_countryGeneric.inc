<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_departmentGeneric.inc 11085 2008-05-28 10:54:49Z hickert $$
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

class country extends department
{
    /* attribute list for save action */
    var $attributes     = array( "c", "ou", "description","gosaUnitTag", "manager");
    var $objectclasses  = array("top", "gosaDepartment");
    var $structuralOC   = array("country");
    var $type   ="country";
    var $c      ="";
    var $orgi_c ="";

    var $namingAttr = "c";
    var $manager_enabled = FALSE;
    var $manager_name ="";
    var $manager ="";


    function check()
    {
        $message = plugin::check();

        /* Check for presence of this department */
        $ldap= $this->config->get_ldap_link();
        $ldap->ls ("(&(c=".$this->c.")(objectClass=country))", $this->base, array('dn'));
        if ($this->orig_c == "new" && $ldap->count()){
            $message[]= msgPool::duplicated(tr("Name"));
        } elseif ($this->orig_dn != $this->dn && $ldap->count()){
            $message[]= msgPool::duplicated(tr("Name"));
        }

        /* All required fields are set? */
        if ($this->c == ""){
            $message[]= msgPool::required(tr("Name"));
        }elseif(tests::is_department_name_reserved($this->c,$this->base)){
            $message[]= msgPool::reserved(tr("Name"));
        }elseif(preg_match ('/[#+:=>\\\\\/]/', $this->c)){
            $message[]= msgPool::invalid(tr("Name"), $this->c, "/[^#+:=>\\\\\/]/");
        }

        /* Check description */
        if ($this->description == ""){
            $message[]= msgPool::required(tr("Description"));
        }

        /* Check if we are allowed to create or move this object
         */
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
            $message[] = msgPool::permCreate();
        }elseif($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
            $message[] = msgPool::permMove();
        }

        return($message);
    }


    /* Return plugin informations for acl handling */ 
    static function plInfo()
    {
        return (array(
                    "plShortName"   => tr("Country"),
                    "plDescription" => tr("Country"),
                    "plSelfModify"  => FALSE,
                    "plPriority"    => 2,
                    "plDepends"     => array(),
                    "plSection"     => array("administration"),
                    "plCategory"    => array("department"),

                    "plProvidedAcls" => array(
                        "c"                 => tr("Country name"),
                        "description"       => tr("Description"),
                        "manager"                 => tr("Manager"),
                        "base"              => tr("Base"),
                        "gosaUnitTag"       => tr("Administrative settings"))
                    ));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
