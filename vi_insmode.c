#include "vi.h"

gvoid insert_chars(ghcar, gchar *);

gvoid
insertmode_main(gchar command, gchar *chars)
{
  
  switch(command) {

    case 'i':
      if(strlen(chs) > 0) insert_chars('b', chars); /* 'b' == before cursor */
      else insert_chars('b', "");
      break;

    case 'I':
      move_cursor_before_line();
      insert_chars('b', "");
      break;

    case 'a':
      move_cursor_over_one();
      insert_chars('a', ""); /* 'a' == after cursor */
      break;

    case 'A':
      move_cursor_after_line();
      insert_chars('a', "");
      break;

    case 'o':
      open_newline_after_current_line();
      insert_chars('b', "");
      break;

    case 'O':
      open_newline_before_current_line();
      insert_chars('b', "");
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
