/*
*   This content is licensed according to the W3C Software License at
*   https://www.w3.org/Consortium/Legal/2015/copyright-software-and-document
*/
/**
 * ARIA Scrollable Listbox Example
 * @function onload
 * @desc Initialize the listbox example once the page has loaded
 */

window.addEventListener('load', function () {
  var exListbox = new aria.Listbox(document.getElementById('ss_elem_list'));

  exListbox.enableStart(document.getElementById('ex1-start'));

  exListbox.enableFocusNext( document.getElementById('ex1-down'));

  exListbox.enableFocusPrev( document.getElementById('ex1-up'));
  
  // exListbox.enableMoveUpDown(
  //   document.getElementById('ex1-up'),
  //   document.getElementById('ex1-down')
  // );

});
