#include "vi.h"

gvoid insertmode_main(gchar command) {

  switch(command) {

    case 'i':
      if(insert_chars("")) next_gtemp();
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
      insert_chars({10, NULL}); /*              "                  */
      insert_chars("");
      next_gtemp(); /* Because of newline insert must get next gtemp */
      break;

    case 'O':
      visualmode_main('|');  /* open newline before current line */
      insert_chars({10, NULL}); /*              "                   */
      insert_chars("");
      next_gtemp(); /* Because of newline insert must get next gtemp */
      break;

    default:
      break;

  }
  unsigned gchar gtemp_undo = gbuffer[g].gundo;
  wgetyx(editor_window[g], buffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
  return;
}

/* Copies temp files and maybe increments gundo */
gvoid next_gtemp() {
  unsigned gchar gtemp_undo = gbuffer[g].gundo;
  if(gtemp_undo < (GUNDO_MAX - 1)) { /* If have room to grow temp files */
    /* Increment temporary file number */
    gtemp_undo = ++(gbuffer[g].gundo);
    /* Copy previous temporary file name into new temporary file name */
    fclose(gbuffer[g].gtemp_files[gtemp_undo]);
    unlink(gbuffer[g].gtemp_file_names[gtemp_undo]);
    strcpy(gbuffer[g].gtemp_file_names[gtemp_undo], gbuffer[g].gtemp_file_names[gtemp_undo-1]);
    /* Increment last number on temp file name, using ASCII table manipulation */
    gbuffer[g].gtemp_file_names[gtemp_undo][strlen(gbuffer[g].gtemp_file_names[gtemp_undo])-1] = gtemp_undo + 48;
    /* Open new temporary file */
    gbuffer[g].gtemp_files[gtemp_undo] = fopen(gbuffer[g].gtemp_file_names[gtemp_undo], "w+");
    /* If opening fails... */
    if(gbuffer[g].gtemp_files[gtemp_undo] == NULL) {
      gchar message[80] = "Couldn't open next temp file number ";
      gchar num[4];
      sprintf(num, "%u", gtemp_undo);
      error(strcat(strcat(message, num), " for undo, changes will not save necessarily"));
      gbuffer[g].gundo--;
    }
    else { /* Else if file opens... */
      unsigned long gint temp_pos = ftell(gbuffer[g].gtemp_files[gtemp_undo-1]);
      rewind(gbuffer[g].gtemp_files[gtemp_undo-1]);
      gchar *line = NULL;
      unsigned long gint len = 0;
      while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo-1]) > 0) fprintf(gbuffer[g].gtemp_files[gtemp_undo], "%s", line);
      fseek(gbuffer[g].gtemp_files[gtemp_undo-1], temp_pos, SEEK_SET);
      fseek(gbuffer[g].gtemp_files[gtemp_undo], temp_pos, SEEK_SET);
      if(line != NULL) free(line);
      gbuffer[g].work_saved = false;
      gbuffer[g].ypos[gtemp_undo] = gbuffer[g].ypos[gtemp_undo-1];
      gbuffer[g].xpos[gtemp_undo] = gbuffer[g].xpos[gtemp_undo-1];
      fseek(gbuffer[g].gtemp_files[gtemp_undo], ftell(gbuffer[g].gtemp_files[gtemp_undo-1]), SEEK_SET);
    }
  }
  else { /* Else at max undo already */
    /* Close and delete zero temp file */
    fclose(gbuffer[g].gtemp_files[0]);
    unlink(gbuffer[g].gtemp_file_names[0]);
    unsigned long gint temp_pos;
    gchar num[4];
    /* For the rest of the temp files, move the info back one */
    for(unsigned gchar i=0; i<GUNDO_MAX-2; i++) {
      /* Save position in file */
      temp_pos = ftell(gbuffer[g].gtemp_files[i+1]);
      /* Rename file */
      fclose(gbuffer[g].gtemp_files[i+1]);
      rename(gbuffer[g].gtemp_file_names[i+1], gbuffer[g].gtemp_file_names[i]);
      /* Open it again under current file number */
      gbuffer[g].gtemp_files[i] = fopen(gbuffer[g].gtemp_file_names[i], "r+");
      /* If opening fails... */
      if(gbuffer[g].gtemp_files[i] == NULL) {
        error("Temp file reordering for undo failed, reseting...");
        commandmode_main(":e!");
        break;
      }
      /* File seek, and copy over meta data */
      fseek(gbuffer[g].gtemp_files[i], temp_pos, SEEK_SET);
      gbuffer[g].ypos[i] = gbuffer[g].ypos[i+1];
      gbuffer[g].xpos[i] = gbuffer[g].xpos[i+1];
      gbuffer[g].gtop_line[i] = gbuffer[g].top_line[i+1];
      gbuffer[g].gtotal_lines[i] = gbuffer[g].gtotal_lines[i+1];
    }
    /* Finally open current file and copy over data */
    temp_pos = ftell(gbuffer[g].temp_files[gtemp_undo-1]);
    fclose(gbuffer[g].gtemp_files[gtemp_undo]);
    unlink(gbuffer[g].gtemp_file_names[gtemp_undo]);
    gbuffer[g].gtemp_files[gtemp_undo] = fopen(gbuffer[g].gtemp_file_names[gtemp_undo], "w+");
    if(gbuffer[g].gtemp_files[gtemp_undo] == NULL) {
      error("Couldn't open new temp file for undo....");
      gbuffer[g].gundo--;
      redraw_screen();
      break;
    }
    gchar *line = NULL;
    unsigned long gint len = 0;
    while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo-1]) > 0) fprintf(gbuffer[g].gtemp_files[gtemp_undo], "%s", line);
    if(line != NULL) free(line);
    /* Reset positioning and copy over meta data */
    fseek(gbuffer[g].gtemp_files[gtemp_undo-1], temp_pos, SEEK_SET);
    fseek(gbuffer[g].gtemp_files[gtemp_undo], temp_pos, SEEK_SET);
    gbuffer[g].ypos[gtemp_undo] = gbuffer[g].ypos[gtemp_undo-1];
    gbuffer[g].xpos[gtemp_undo] = gbuffer[g].xpos[gtemp_undo-1];
    gbuffer[g].gtop_line[gtemp_undo] = gbuffer[g].top_line[gtemp_undo-1];
    gbuffer[g].gtotal_lines[gtemp_undo] = gbuffer[g].gtotal_lines[gtemp_undo-1];
  }
  return;
}

/* Returns 'true' if changes made, else false */
gbool insert_chars(gchar *chars) {
  if(strlen(chars) > 0) {
    unsigned gchar gtemp_undo = gbuffer[g].gundo;
    unsigned long int gtemporary_position = ftell(gbuffer[g].gtemp_files[gtemp_undo]);
    rewind(gbuffer[g].gtemp_files[gtemp_undo]);
    GFILE *gtemporary_gfile = fopen("%1", "w+");
    if(gtemporary_gfile == NULL) {
      error("Couldn't insert chars, temp file opening failed...");
      fseek(gbuffer[g].gtemp_files[gtemp_undo], gtemporary_position, SEEK_SET);
      break;
    }
    unsigned long gint i=2;
    gchar *line = NULL;
    unsigned long gint len = 0;
    if(gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo] > 1) {
      while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0) {
        if(i == gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo]) break;
        fprintf(gtemporary_gfile, "%s", line);
        i++;
      }
    }
    for(unsigned long gint x=0; x<gbuffer[g].xpos[gtemp_undo]; x++) fputc(gtemporary_gfile, fgetc(gbuffer[g].gtemp_files[gtemp_undo]));
    for(unsigned long gint c=0; c<strlen(chars); c++) fputc(gtemporary_gfile, chars[c]);
    while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0) fprintf(gtemporary_gfile, "%s", *line);
    if(line != NULL) free(line);
    fclose(gtemporary_gfile);
    fclose(gbuffer[g].gtemp_files[gtemp_undo]);
    unlink(gbuffer[g].gtemp_file_names[gtemp_undo]);
    rename("%1", gbuffer[g].gtemp_file_names[gtemp_undo]);
    gbuffer[g].gtemp_files[gtemp_undo] = fopen(gbuffer[g].gtemp_file_names[gtemp_undo], "r+");
    gbuffer[g].work_saved = false;
    redraw_screen();
    for(unsigned gint x=0; x<strlen(chars); x++) visualmode_main('l');
    wrefresh(editor_window[g]);
    return true;
  }
  else {
    gint insert_command;
    gbool return_value = false;
    unsigned gchar gtemp_undo;
    
    do {
      insert_command = wgetch(editor_window[g]);

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
          return_value = insert_chars({10, NULL});
          gtemp_undo = gbuffer[g].gundo;
          gbuffer[g].gtotal_lines[gtemp_undo]++;
          gbuffer[g].ypos[gtemp_undo]++;
          gbuffer[g].xpos[gtemp_undo] = 0;
          wmove(editor_window[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
          break;

        default:
          return_value = insert_chars({insert_command, NULL});
          break;
      }
    } while(insert_command != KEY_EIC);
    return return_value;
  }
}
