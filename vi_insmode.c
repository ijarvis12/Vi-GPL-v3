#include "vi.h"

gbool insert_chars(gchar *);

gvoid insertmode_main(gchar command, gchar *chars) {

  switch(command) {

    case 'i':
      insert_chars(chars); /* chars can be empty ("") */
      break;

    case 'I':
      visualmode_main('^'); /* move cursor to start of line */
      if(insert_chars("")) next_gtemp();
      break;

    case 'a':
      visualmode_main('l');
      if(insert_chars("")) next_gtemp();
      break;

    case 'A':
      visualmode_main('$'); /* move cursor to end of line */
      if(insert_chars("")) next_gtemp();
      break;

    case 'o':
      visualmode_main('$');  /* open newline after current line */
      insert_chars({10, 0}); /*              "                  */
      insert_chars("");
      next_gtemp(); /* Because of newline insert must get next gtemp */
      break;

    case 'O':
      visualmode_main('|');  /* open newline before current line */
      insert_chars({10, 0}); /*              "                   */
      insert_chars("");
      next_gtemp(); /* Because of newline insert must get next gtemp */
      break;

    default:
      break;

  }

  wgetyx(editor_window[g], ypos[g], xpos[g]);
  return;
}

gbool insert_chars(gchar *chars) { /* Returns 'true' if changes made, else false */


  work_saved[g] = false;
  return true;
}
