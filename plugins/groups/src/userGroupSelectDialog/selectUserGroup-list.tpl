<div id="mainlist">

  <div class="mainlist-header">
   <p>{$HEADLINE}&nbsp;{$SIZELIMIT}</p>
   <div class="mainlist-nav">
    <table summary="{$HEADLINE}">
     <tr>
      <td>{$ROOT}</td>
      <td>{$BACK}</td>
      <td>{$HOME}</td>
      <td>{$RELOAD}</td>
      <td class="left-border">{tr}Base{/tr} {$BASE}</td>
      <td class="left-border">{$ACTIONS}</td>
      <td class="left-border">{$FILTER}</td>
     </tr>
    </table>
   </div>
  </div>

  {$LIST}
</div>

<div class="clear"></div>

<div class="plugin-actions">
  <button type="submit" name="userGroupSelect_save">{msgPool type=okButton}</button>
  <button type="submit" name="userGroupSelect_cancel">{msgPool type=cancelButton}</button>
</div>

