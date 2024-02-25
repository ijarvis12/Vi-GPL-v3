#include "vi.h"

gvoid insert_chars(gchar *);

gvoid insertmode_main(gchar command, gchar *chars)
{
  
  switch(command) {

    case 'i':
      insert_chars(chars); /* chars can be empty ("") */
      break;

    case 'I':
      visualmode_main('^'); /* move cursor to start of line */
      insert_chars("");
      break;

    case 'a':
      visualmode_main('l');
      insert_chars("");
      break;

    case 'A':
      visualmode_main('$'); /* move cursor to end of line */
      insert_chars("");
      break;

    case 'o':
      visualmode_main('$'); /* open newline after current line */
      insert_chars({10, 0});   /*              "                  */
      insert_chars("");
      break;

    case 'O':
      visualmode_main('-'); /* open newline before current line */
      visualmode_main('$'); /*              "                   */
      insert_chars({10, 0});   /*              "                   */
      insert_chars("");
      break;

    default:
      break;

  }

  work_saved[g] = false;
  wgetyx(editor_window[g], ypos[g], xpos[g]);

}

gvoid insert_chars(ghcar *chars)
{

}
