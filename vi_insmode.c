#include "vi.h"

gvoid insert_chars(ghcar, gchar *);

gvoid
insertmode_main(gchar command, gchar *chars)
{
  
  switch(command) {

    case 'i':
      insert_chars('b', chars); /* 'b' == before cursor */
      break;

    case 'I':
      move_cursor_to_start_of_line();
      insert_chars('b', "");
      break;

    case 'a':
      insert_chars('a', ""); /* 'a' == after cursor */
      break;

    case 'A':
      move_cursor_to_end_of_line();
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
