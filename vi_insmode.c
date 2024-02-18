#include "vi.h"

gvoid insert_chars(ghcar, gchar *);

gvoid
insertmode_main(gchar command, gchar *chars)
{
  
  switch(command) {

    case 'i':
      insert_chars('b', chars); /* 'b' == before cursor; chars can be "" */
      break;

    case 'I':
      visualmode_main('^'); /* move cursor to start of line */
      insert_chars('b', "");
      break;

    case 'a':
      insert_chars('a', ""); /* 'a' == after cursor */
      break;

    case 'A':
      visualmode_main('$'); /* move cursor to end of line */
      insert_chars('a', "");
      break;

    case 'o':
      visualmode_main('$'); /* open newline after current line */
      insert_chars('a', "\n");
      insert_chars('b', "");
      break;

    case 'O':
      visualmode_main('-'); /* open newline before current line */
      visualmode_main('$');
      insert_chars('a', "\n");
      insert_chars('b', "");
      break;

    case 'R':
      replace_many_chars(); /* ***TODO*** */
      break;

    default:
      break;

  }

  work_saved[g] = false;
  wgetyx(editor_window[g], ypos[g], xpos[g]);

}
