
{if !$mail_account}
	<h3>{tr}Kolab account{/tr}</h3>
	{tr}The Kolab account is currently disabled. It's features can be adjusted if you add a mail account.{/tr}
{else}

<h3>
{if $multiple_support}

<input type="checkbox" name="use_kolabState" value="1" {if $use_kolabState} checked {/if}
	class="center" onClick="changeState('kolabState');">
<input type="checkbox" id="kolabState" name="kolabState" value="1" {if $kolabState} checked {/if}
    class="center" {if  !$use_kolabState} disabled {/if}>

{else}
<input type="checkbox" id="kolabState" name="kolabState" value="1" {if $kolabState} checked {/if}
	class="center" 
	{if (!$is_account && $is_createable) || ($is_account && $is_removeable)}
	{else}
		disabled 	
	{/if}
	onClick="
	{if $kolabDelegate_W}
	changeState('delegate_list');
	changeState('delegate_address');
	changeState('add_delegation');
	changeState('delete_delegation');
	{/if}
	{if $unrestrictedMailSize_W}
	changeState('unrestrictedMailSize');
	{/if}
	{if $calFBURL_W}
	changeState('calFBURL');
	{/if}
	{if $kolabFreeBusyFuture_W}
	changeState('kolabFreeBusyFuture');
	{/if}
	{if $kolabInvitationPolicy_W}
	{$changeState}
	{/if}">
{/if}

{tr}Kolab account{/tr}</h3>
<table style="width:100%" summary="{tr}Kolab delegation configuration{/tr}">
 <tr>
  <td style='width:50%; '>


{if $multiple_support}
	<input type="checkbox" name="use_kolabDelegate" {if $use_kolabDelegate} checked {/if}
   		class="center" onClick="changeState('delegate_list');">
   	<b><LABEL for="delegate_list">{tr}Delegations{/tr}</LABEL></b><br>
	<select id="delegate_list" style="width:350px; height:100px;" name="delegate_list[]" size=15 multiple 
		{if !$use_kolabDelegate}disabled{/if} >
		{html_options values=$kolabDelegate output=$kolabDelegate}
		<option disabled>&nbsp;</option>
	</select>
   <br>
   <input type='text' name="delegate_address" size=30 align=middle maxlength=60 value="" id="delegate_address">
   <button type='submit' name='add_delegation' id="add_delegation">{msgPool type=addButton}</button>&nbsp;

   <button type='submit' name='delete_delegation' id="delete_delegation">{msgPool type=delButton}</button>


{else}
   <b><LABEL for="delegate_list">{tr}Delegations{/tr}</LABEL></b><br>
	{render acl=$kolabDelegateACL}
	   <select id="delegate_list" style="width:350px; height:100px;" name="delegate_list[]" size=15 multiple 
		 {if !$kolabState}disabled{/if} >
		{html_options values=$kolabDelegate output=$kolabDelegate}
		<option disabled>&nbsp;</option>
	   </select>
	{/render}
	   <br>
	{render acl=$kolabDelegateACL}
	   <input type='text' name="delegate_address" size=30 align=middle maxlength=60 {if !$kolabState} disabled {/if} value="" id="delegate_address">
	{/render}
	{render acl=$kolabDelegateACL}
	   <button type='submit' name='add_delegation' id="add_delegation" {if !$kolabState} disabled {/if}
>{msgPool type=addButton}</button>&nbsp;

	{/render}
	{render acl=$kolabDelegateACL}
	   <button type='submit' name='delete_delegation' id="delete_delegation" {if !$kolabState} disabled {/if}
>{msgPool type=delButton}</button>

	{/render}
{/if}

    <h3>{tr}Mail size{/tr}</h3>
{render acl=$unrestrictedMailSizeACL checkbox=$multiple_support checked=$use_unrestrictedMailSize}
     &nbsp;<input class="center" type="checkbox" id='unrestrictedMailSize' name="unrestrictedMailSize" value="1" 
		{if !$kolabState && !$multiple_support} disabled {/if} {$unrestrictedMailSizeState}> 
	{tr}No mail size restriction for this account{/tr}
{/render}
  </td>
  <td class='left-border' rowspan="2">

   &nbsp;
  </td>
  <td>


 <h3>{tr}Free Busy information{/tr}</h3>
 <table summary="{tr}Free Busy information{/tr}">
  <tr>
   <td><LABEL for="calFBURL">{tr}URL{/tr}</LABEL></td>
   <td>
{render acl=$calFBURLACL checkbox=$multiple_support checked=$use_calFBURL}
   <input type='text' id="calFBURL" name="calFBURL" size=30 maxlength=60 value="{$calFBURL}" 
	{if !$kolabState && !$multiple_support} disabled {/if}>
{/render}
   </td>
  </tr>
  <tr>
  <td><LABEL for="kolabFreeBusyFuture">{tr}Future{/tr}</LABEL></td>
  <td>
{render acl=$kolabFreeBusyFutureACL checkbox=$multiple_support checked=$use_kolabFreeBusyFuture}
   <input type='text' id="kolabFreeBusyFuture" name="kolabFreeBusyFuture" size=5 maxlength=6 
	{if !$kolabState && !$multiple_support} disabled {/if} value="{$kolabFreeBusyFuture}" > 
	{tr}days{/tr}
{/render}
   </td>
  </tr>
 </table>


<h3>{tr}Invitation policy{/tr}</h3>
{if $multiple_support}
<input type="checkbox" name="use_kolabInvitationPolicy" {if $use_kolabInvitationPolicy} checked {/if} value="1" class="center">
{/if}
{render acl=$kolabInvitationPolicyACL}
 <table summary="{tr}Invitation policy{/tr}">
   {$invitation}
 </table>
{/render}


  </td>
 </tr>
</table>

{/if}
