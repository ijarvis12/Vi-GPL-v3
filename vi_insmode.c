#include "vi.h"

gvoid next_gtemp();
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

/* Copies temp files and maybe increments gtemp[g] */
gvoid next_gtemp() { 
  if(gtemp[g] < (GUNDO_MAX - 1)) {
    gtemp[g]++;
    strcpy(temp_file_names[g][gtemp[g]], temp_file_names[g][gtemp[g]-1]);
    strcat(temp_file_names[g][gtemp[g]], "%");
    temp_files[g][gtemp[g]] = fopen(temp_file_names[g][gtemp[g]]);
    if(temp_files[g][gtemp[g]] == NULL) {
      gchar message[255] = "Couldn't open next temp file number ";
      gchar num[8];
      sprintf(num, "%u", gtemp[g]);
      error(strcat(strcat(message, num), " for undo, changes will not save"));
      free(message);
      free(num);
    }
    else { /* Else */

    }
  }
  else {

  }
  return;
}

/* Returns 'true' if changes made, else false */
gbool insert_chars(gchar *chars) {


  work_saved[g] = false;
  return true;
}
