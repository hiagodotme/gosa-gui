<h3>{tr}Mail settings{/tr}</h3>
<table style='width:100%; ' summary="{tr}Paste mail settings{/tr}">

 	<tr>
  		<td colspan=2>   
   			<table summary="{tr}Address settings{/tr}">
    			<tr>
     				<td>
						<LABEL for="mail">{tr}Primary address{/tr}</LABEL>{$must}
					</td>
     				<td>
						<input id="mail" name="mail" size="25" maxlength="65" type='text' 
							value="{$mail}" title="{tr}Primary mail address for this shared folder{/tr}">
					</td>
    			</tr>
   			</table>
  		</td>
 	</tr>
 	<tr>
		<td>
			<table style='width:100%;' summary="{tr}Additional mail settings{/tr}">

				<tr>
					<td style='width:50%; ' class='right-border'>

							{tr}Alternative addresses{/tr}
						<select style="width:100%;" name="alternates_list[]" size=10 multiple title="{tr}List of alternative mail addresses{/tr}">
							{html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
							<option disabled>&nbsp;</option>
						</select>
						<br>
						<input type='text' name="alternate_address" size="30" align=middle maxlength="60" value="">
						<button type='submit' name='add_alternate'>{msgPool type=addButton}</button>&nbsp;

						<button type='submit' name='delete_alternate'>{msgPool type=delButton}</button>

					</td>
					<td style='width:50%'>

							{tr}Forward messages to non group members{/tr}
						<select style="width:100%;" name="forwarder_list[]" size=10 multiple>
							{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
							<option disabled>&nbsp;</option>
						</select>
						<br>
						<input type='text' name="forward_address" size=20 align=middle maxlength=65 value="">
						<button type='submit' name='add_forwarder'>{msgPool type=addButton}</button>&nbsp;

						<button type='submit' name='add_local_forwarder'>{tr}Add local{/tr}</button>&nbsp;

						<button type='submit' name='delete_forwarder'>{msgPool type=delButton}</button>

					</td>
				</tr>
			</table>
		<td>
	</tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('mail');
  -->
</script>
