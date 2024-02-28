#include "vi.h"

gvoid next_gtemp();
gbool insert_chars(gchar **);

gvoid insertmode_main(gchar command) {

  switch(command) {

    case 'i':
      if(insert_chars("")) next_gtemp(); /* chars can be empty pointer ("") */
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

  wgetyx(editor_window[g], buffer[g].ypos[gtemp[g]], gbuffer[g].xpos[gtemp[g]]);
  return;
}

/* Copies temp files and maybe increments gtemp[g] */
gvoid next_gtemp() { 
  if(gtemp[g] < (GUNDO_MAX - 1)) {
    /* Increment temporary file number */
    gtemp[g]++;
    /* Copy previous temporary file name into new temporary file name */
    strcpy(gbuffer[g].gtemp_file_names[gtemp[g]], gbuffer[g].gtemp_file_names[gtemp[g]-1]);
    /* Increment last number on temp file name */
    gbuffer[g].gtemp_file_names[gtemp[g]][strlen(gbuffer[g].gtemp_file_names[gtemp[g]])-1] = gtemp[g];
    /* Open new temporary file */
    gbuffer[g].gtemp_files[gtemp[g]] = fopen(gbuffer[g].gtemp_file_names[gtemp[g]], "rw");
    /* If opening fails... */
    if(gbuffer[g].gtemp_files[gtemp[g]] == NULL) {
      gchar message[80] = "Couldn't open next temp file number ";
      gchar num[4];
      sprintf(num, "%u", gtemp[g]);
      error(strcat(strcat(message, num), " for undo, changes will not save"));
    }
    else { /* Else if file opens... */
      unsigned long gint temp_pos = ftell(gbuffer[g].gtemp_files[gtemp[g]-1]);
      gchar **line;
      while(getline(line, NULL, gbuffer[g].gtemp_files[gtemp[g]-1]) > 0) fprintf(gbuffer[g].gtemp_files[gtemp[g]], "%s", *line);
      fseek(gbuffer[g].gtemp_files[gtemp[g]-1], temp_pos, SEEK_SET);
      fseek(gbuffer[g].gtemp_files[gtemp[g]], temp_pos, SEEK_SET);
      free(line);
      gbuffer[g].ypos[gtemp[g]] = gbuffer[g].ypos[gtemp[g]-1];
      gbuffer[g].xpos[gtemp[g]] = gbuffer[g].xpos[gtemp[g]-1];
      fseek(gbuffer[g].gtemp_files[gtemp[g]], ftell(gbuffer[g].gtemp_files[gtemp[g]-1]), SEEK_SET);
    }
  }
  else {
    /* Close and delete zero temp file */
    fclose(gbuffer[g].gtemp_files[0]);
    unlink(gbuffer[g].gtemp_file_names[0]);
    unsigned long gint temp_pos;
    gchar num[4];
    /* For the rest of the temp files, move the info back one */
    for(unsigned gchar i=0; i<GUNDO_MAX-2; i++) {
      /* Save position in file */
      temp_pos = ftell(gbuffer[g].gtemp_files[i+1]);
      /* Copy name over, and give it the right name */
      strcpy(gbuffer[g].gtemp_file_names[i], gbuffer[g].gtemp_file_names[i+1]);
      sprintf(num, "%u", i);
      gbuffer[g].gtemp_file_names[i][strlen(gbuffer[g].gtemp_file_names[i])-1] = num;
      rename(gbuffer[g].gtemp_file_names[i+1], gbuffer[g].gtemp_file_names[i]);
      /* Close incremented file and open it again under current file number */
      fclose(gbuffer[g].gtemp_files[i+1]);
      gbuffer[g].gtemp_files[i] = fopen(gbuffer[g].gtemp_file_names[i], "rw");
      /* Copy data over and file seek */
      fseek(gbuffer[g].gtemp_files[i], temp_pos, SEEK_SET);
      gbuffer[g].ypos[i] = gbuffer[g].ypos[i+1];
      gbuffer[g].xpos[i] = gbuffer[g].xpos[i+1];
      gbuffer[g].gtop_line[i] = gbuffer[g].top_line[i+1];
      gbuffer[g].gtotal_lines[i] = gbuffer[g].gtotal_lines[i+1];
      
    }
    /* Finally open current file and copy over data */
    temp_pos = ftell(gbuffer[g].temp_files[gtemp[g]-1]);
    gbuffer[g].gtemp_files[gtemp[g]] = fopen(gbuffer[g].gtemp_file_names[gtemp[g]], "rw");
    gchar **line;
    while(getline(line, NULL, gbuffer[g].gtemp_files[gtemp[g]-1]) > 0) fprintf(gbuffer[g].gtemp_files[gtemp[g]], "%s", *line);
    free(line);
    fseek(gbuffer[g].gtemp_files[gtemp[g]-1], temp_pos, SEEK_SET);
    fseek(gbuffer[g].gtemp_files[gtemp[g]], temp_pos, SEEK_SET);
    gbuffer[g].ypos[gtemp[g]] = gbuffer[g].ypos[gtemp[g]-1];
    gbuffer[g].xpos[gtmep[g]] = gbuffer[g].xpos[gtemp[g]-1];
    gbuffer[g].gtop_line[gtemp[g]] = gbuffer[g].top_line[gtemp[g]-1];
    gbuffer[g].gtotal_lines[gtemp[g]] = gbuffer[g].gtotal_lines[gtemp[g]-1];
  }
  return;
}

/* Returns 'true' if changes made, else false */
gbool insert_chars(gchar *chars) {


  gbuffer[g].work_saved = false;
  return true;
}
