#include "vi.h"

gvoid insert_chs_after_cursor(gchar *);

gvoid
insertmode_main(gchar cmd, gchar *chs)
{
  
  switch(cmd) {

    case 'i':
      if(strlen(chs) > 0) insert_chs('b', chs);
      else insert_chs('b', ""); /* 'b' == before cursor */
      break;

    case 'I':
      move_cursor_before_line();
      insert_chs('b', "");
      break;

    case 'a':
      move_cursor_over_one();
      insert_chs('a', ""); /* 'a' == after cursor */
      break;

    case 'A':
      move_cursor_after_line();
      insert_chs('a', "");
      break;

    case 'o':
      open_newline_after_current_line();
      insert_chs('b', "");
      break;

    case 'O':
      open_newline_before_current_line();
      insert_chs('b', "");
      break;

    case 'R':
      replace_many_chs();
      break;

    default:
      break;

  }

  work_saved[g] = false;
  wgetyx(editor_window[g], ypos[g], xpos[g]);

}
