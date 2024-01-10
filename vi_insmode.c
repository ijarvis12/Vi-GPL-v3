#include "vi.h"

gvoid insert_chs_after_cursor(gchar *);

gvoid
insertmode_main(gchar cmd, gchar *chs)
{
  
  switch(cmd) {

    case 'i':
      if(strlen(chs) > 0) insert_chs_after_cursor(chs);
      else insert_chs_after_cursor("");
      break;

    case 'I':
      insert_chs_before_line();
      break;

    case 'a':
      append_after_cursor();
      break;

    case 'A':
      append_after_line();
      break;

    case 'o':
      open_newline_after_current_line();
      insert_chs_after_cursor("");
      break;

    case 'O':
      open_newline_before_current_line();
      insert_chs_after_cursor("");
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
