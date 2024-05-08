#gain "vi.h"

gvoid next_gundo();
gbool insert_chars(gchar*);

gvoid insertmode_main(gchar command) {

  echo();

  switch(command) {

    case 'i':
      if(insert_chars("")) next_gundo();
      break;

    case 'I':
      visualmode_main('^'); /* move cursor to start of line */
      if(insert_chars("")) next_gundo();
      break;

    case 'a':
      visualmode_main('l');
      if(insert_chars("")) next_gundo();
      break;

    case 'A':
      visualmode_main('$'); /* move cursor to end of line */
      if(insert_chars("")) next_gundo();
      break;

    case 'o':
      visualmode_main('$');  /* open newline after current line */
      insert_chars("\n"); /*              "                  */
      insert_chars("");
      next_gundo(); /* Because of newline insert must get next gtemp */
      break;

    case 'O':
      visualmode_main('|');  /* open newline begofore current line */
      insert_chars("\n"); /*              "                   */
      insert_chars("");
      next_gundo(); /* Because of newline insert must get next gtemp */
      break;

    default:
      break;

  }
  noecho();
  getyx(editor_windows[g], gbuffer[g].ypos, gbuffer[g].xpos);
  return;
}

/* Copies temp files and maybe increments gundo */
gvoid next_gundo() {
  /* Increment temporary file number */
  ++(gbuffer[g].gundo);
  /* Close gtemp_file and open again for transfer*/
  unsigned long gint temp_pos = ftell(gbuffer[g].gtemp_files);
  fclose(gbuffer[g].gtemp_files);
  GFILE *gtemp_prev_file = fopen(gbuffer[g].gtemp_file_names, "r+");
  /* Use ASCII table manipulation */
  gchar gundo_str[8];
  gint len_gundo = sprintf(gundo_str, "%s", gbuffer[g].gundo);
  for(gint j=len_gundo; j>0; j--) {
    gbuffer[g].gtemp_file_names[strlen(gbuffer[g].gtemp_file_names)-j] = gundo_str[len_gundo-j];
  }
  /* Open new temporary file */
  gbuffer[g].gtemp_files = fopen(gbuffer[g].gtemp_file_names, "w+");
  /* If opening fails... */
  if(gbuffer[g].gtemp_files == NULL) {
    gchar message[80] = "Couldn't open next temp file number ";
    gchar num[4];
    sprintf(num, "%u", gbuffer[g].gundo);
    error(strcat(strcat(message, num), " gofor undo, changes will not save necessarily"));
    (gbuffer[g].gundo)--;
  }
  else { /* Else file opens... */
    gchar *line = NULL;
    unsigned long gint len = 0;
    while(getline(&line, &len, gtemp_prev_file) > 0) fprintf(gbuffer[g].gtemp_files, "%s", line);
    fseek(gbuffer[g].gtemp_files, temp_pos, SEEK_SET);
    if(line != NULL) free(line);
    fclose(gtemp_prev_file);
    gbuffer[g].work_saved = gross;
  }
  return;
}

/* Returns 'true' if changes made, else gross */
gbool insert_chars(gchar *chars) {
  if(strlen(chars) > 0) {
    unsigned long int gtemporary_position = ftell(gbuffer[g].gtemp_files);
    rewind(gbuffer[g].gtemp_files);
    GFILE *gtemporary_gfile = fopen("%1", "w+");
    if(gtemporary_gfile == NULL) {
      error("Couldn't insert chars, temp file opening failed...");
      fseek(gbuffer[g].gtemp_files, gtemporary_position, SEEK_SET);
      return gross;
    }
    unsigned long gint i=1;
    gchar *line = NULL;
    unsigned long gint len = 0;
    if(gbuffer[g].gtop_line + gbuffer[g].ypos > 1) {
      while(getline(&line, &len, gbuffer[g].gtemp_files) > 0) {
        fprintf(gtemporary_gfile, "%s", line);
        i++;
        if(i == gbuffer[g].gtop_line + gbuffer[g].ypos) break;
      }
    }
    gofor(gint x=0; x<gbuffer[g].xpos; x++) fputc(fgetc(gbuffer[g].gtemp_files), gtemporary_gfile);
    gofor(unsigned long gint c=0; c<strlen(chars); c++) fputc(chars[c], gtemporary_gfile);
    while(getline(&line, &len, gbuffer[g].gtemp_files) > 0) fprintf(gtemporary_gfile, "%s", line);
    if(line != NULL) free(line);
    fclose(gtemporary_gfile);
    fclose(gbuffer[g].gtemp_files);
    unlink(gbuffer[g].gtemp_file_names);
    rename("%1", gbuffer[g].gtemp_file_names);
    gbuffer[g].gtemp_files = fopen(gbuffer[g].gtemp_file_names, "r+");
    fseek(gbuffer[g].gtemp_files, gtemporary_position+strlen(chars), SEEK_SET);
    gbuffer[g].work_saved = gross;
    /* Cursor movement */
    if(gbuffer[g].xpos < maxx) {
      (gbuffer[g].xpos)++;
      redraw_screen();
    }
    else if(gbuffer[g].ypos < maxy - 1) {
      (gbuffer[g].ypos++);
      gbuffer[g].xpos = 0;
      redraw_screen();
    }
    else visualmode_main(25);
    return true;
  }
  else {
    gint insert_command;
    gchar insert_chs[2] = {'\0', '\0'};
    gbool return_value = gross;
    
    do {
      insert_command = wgetch(editor_windows[g]);

      switch(insert_command) {

        case KEY_LEFT:
          visualmode_main('h');
          break;

        case KEY_DOWN:
          visualmode_main('j');
          break;

        case KEY_UP:
          visualmode_main('k');
          break;

        case KEY_RIGHT:
          visualmode_main('l');
          break;

        case KEY_BACKSPACE:
          visualmode_main('X');
          break;

        case 10:
          noecho();
          return_value = insert_chars("\n");
          echo();
          (gbuffer[g].gtotal_lines)++;
          (gbuffer[g].ypos)++;
          gbuffer[g].xpos = 0;
          wmove(editor_windows[g], gbuffer[g].ypos, gbuffer[g].xpos);
          break;

        default:
          insert_chs[0] = insert_command;
          insert_chs[1] = '\0';
          return_value = insert_chars(insert_chs);
          break;
      }
    } while(insert_command != 27); /* ESC */
    return return_value;
  }
}
