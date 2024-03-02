#include "vi.h"
#include "vi_re.h"

gvoid write_to_file(gchar *); /* Write file to storage */
gvoid quit();                  /* Quit out of current file buffer, and maybe program */

/* Main entry point for command mode */
gvoid commandmode_main(gchar *command) {
  /* Get command from window/field if no input command to calling function*/
  if(strlen(command) == 0) {
    mvwhline(command_window, 0, 0, ' ', maxx);
    mvwgetnstr(command_window, 0, 0, command, maxx); /* from ncurses */
  }
  
  /* First a sanity check */
  if(strlen(command) == 0) return;

  /* Process command */
  switch (command[0]) { /* Switch on the first character */

    /* Show filename */
    case 7: /* Ctrl-g */
      if(strlen(gbuffer[g].gfile_name) > 0) print(gbuffer[g].gfile_name);
      else error("No filename specified");
      commandmode_main(":.=");
      commandmode_main(":=");
      unsigned gchar gtemp_undo = gbuffer[g].gundo;
      unsigned long gint percent = (100*(gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo])) / gbuffer[g].gtotal_lines[gtemp_undo];
      gchar percent_str[24];
      sprintf(percent_str, "%u", percent);
      print(strcat(percent_str, "% through file"));
      break;

    /* Write and quit */
    case 'Z':
      /* ZZ */
      if(strlen(command) == 2 && command[1] == 'Z') {
        if(strlen(gbuffer[g].gfile_name > 0) {
          write_to_file("");
          quit();
        }
        else error("No filename specified");
      }
        
      else error("Command not recognized");
      break;

    /* Command */
    case ':':
      /* Do rest of command */
      unsigned gchar len_command = strlen(command);
      if(len_command == 1) break; /* First, a sanity check */
      
      switch (command[1]) { /* Switch on the second char of the command */
        
        /* Write and quit */
        case 'x':
          /* :x */
          if(strlen(gbuffer[g].gfile_name > 0) {
            write_to_file("");
            quit();
          }
          else error("No filename specified");
          break;
        
        /* Write, maybe quit too if specified */
        case 'w':
          /* :w [file] */
          if(len_command > 3 && command[2] == ' ') {
            gchar gfile_name[len_command-2];
            for(unsigned gchar i=3; i<len_command; i++) gfile_name[i-3] = command[i];
            write_to_file(gfile_name);
            gchar message[255] = "File ";
            print(strcat(strcat(message, gfile_name), " saved"));
          }
          /* :w */
          else if(len_command == 2) {
            if(strlen(gbuffer[g].gfile_name > 0) {
              write_to_file("");
              print("Filed saved");
            }
            else error("No filename specified");
          }
          /* :wq */
          else if(len_command == 3 && command[2] == 'q') {
            if(strlen(gbuffer[g].gfile_name > 0) {
              write_to_file("");
              quit();
            }
            else error("No filename specified");
          }
            
          else error("Command not recognized");
          break;

        /* Quit, maybe if everything is saved */
        case 'q':
          /* :q! */
          if(len_command == 3 && command[2] == '!') quit();
          /* :q */
          else if(len_command == 2 && gbuffer[g].work_saved) quit();
          else if(len_command == 2) error("Unsaved work");

          else error("Command not recognized");
          break;

        /* Edit */
        case 'e':
          /* :e! */
          if(len_command == 3 && command[2] == '!') {
            /* Reload from permament file */
            if(strlen(gbuffer[g].gfile_name) > 0) gbuffer[g].gfile = fopen(gbuffer[g].gfile_name, 'r');
            else {error("No file to reload from"); break;}
            if(gbuffer[g].gfile == NULL) error("Couldn't reload file");
            else {
              /* Delete old temp files and make a new temp file */
              for(unsigned gchar i=0; i<GUNDO_MAX; i++) {
                fclose(gbuffer[g].gtemp_files[i]);
                unlink(gbuffer[g].gtemp_file_names[i]);
              }
              unsigned gchar gtemp_undo = gbuffer[g].gundo = 0;
              strcpy(gbuffer[g].gtemp_file_names[gtemp_undo], "/var/tmp/vi/");
              strcat(strcat(strcat(strcat(gbuffer[g].gtemp_file_names[gtemp_undo], gentenv("USER")), "/"), gbuffer[g].gfile_name), "0");
              gbuffer[g].gtemp_files[gtemp_undo] = fopen(gbuffer[g].gtemp_file_names[gtemp_undo], 'w+');
              /* Sanity check */
              if(gbuffer[g].gtemp_files[gtemp_undo] == NULL) {
                error("Temp file could not be opened");
                fclose(gbuffer[g].gfile);
                break; /* ****BREAK***** */
              }
              /* Load permament file into temp */
              gchar *line = NULL;
              unsigned long gint len = 0;
              gbuffer[g].gtotal_lines[gtemp_undo] = 0;
              while(getline(&line, &len, gbuffer[g].gfile) > 0) {
                fprintf(gbuffer[g].gtemp_files[gtemp_undo], "%s", line);
                gbuffer[g].gtotal_lines[gtemp_undo] += 1;
              }
              /* Cleanup and go */
              fclose(gbuffer[g].gfile);
              if(line != NULL) free(line);
              gbuffer[g].work_saved = true;
              gbuffer[g].gtop_line[gtemp_undo] = 1;
              gbuffer[g].ypos[gtemp_undo] = 0;
              gbuffer[g].xpos[gtemp_undo] = 0;
              redraw_screen();
            }
          }
          /* :e [file] */
          else if(len_command > 3 && command[2] == ' ') {
            /* Move to next open buffer */
            unsigned gchar temporary_g = g;
            while(gbuffer[g].buffer_is_open) {
              g++;
              if(g > GMAX_FILES - 1) g = 0;
              else if(g == temporary_g) {error("No more open buffers"); return;}
            }
            /* Maybe load file if it exists */
            for(unsigned gchar i=3; i<len_command+1; i++) gbuffer[g].gfile_name[i-3] = command[i];
            gbuffer[g].gfile = fopen(gbuffer[g].gfile_name, 'r'); /* Okay if fails, usually b/c it's a new file */
            /* Make a new temp file */
            unsigned gchar gtemp_undo = gbuffer[g].gundo = 0;
            strcpy(gbuffer[g].gtemp_file_names[gtemp_undo], "/var/tmp/vi/");
            strcat(strcat(strcat(strcat(gbuffer[g].temp_file_names[gtemp_undo], gentenv("USER")), "/"), gbuffer[g].gfile_name), "0");
            gbuffer[g].gtemp_files[gtemp_undo] = fopen(gbuffer[g].gtemp_file_names[gtemp_undo], 'w+');
            /* Sanity check */
            if(gbuffer[g].gtemp_files[gtemp_undo] == NULL) {
              error("Temp file could not be opened");
              fclose(gbuffer[g].file);
              g = temporary_g;
            }
            else { /* Load permament file into temp, if any to load */
              gchar *line = NULL;
              unsigned long gint len = 0;
              gbuffer[g].gtotal_lines[gtemp_undo] = 0;
              while(getline(&line, &len, gbuffer[g].gfile) > 0) {
                fprintf(gbuffer[g].gtemp_files[gtemp_undo], "%s", line);
                gbuffer[g].gtotal_lines[gtemp_undo] += 1;
              }
              /* Cleanup and go */
              fclose(gbuffer[g].gfile);
              if(line != NULL ) free(line);
              gbuffer[g].work_saved = true;
              gbuffer[g].buffer_is_open = true;
              gbuffer[g].gtop_line[gtemp_undo] = 1;
              gbuffer[g].ypos[gtemp_undo] = 0;
              gbuffer[g].xpos[gtemp_undo] = 0;
              redraw_screen();
            }
          }

          else error("Command not recognized");
          break;

        /* Read file in after current line */
        case 'r':
          /* :r [file] */
          if(len_command > 3 && command[2] == ' ') {
            /* open file */
            gchar file_name[len_command-2];
            for(unsigned gchar i=3; i<len_command+1; i++) file_name[i-3] = command[i];
            GFILE *file = fopen(file_name, 'r');
            if(file == NULL) error("Couldn't load file");
            else {
              /* Insert file */
              gchar *line = NULL;
              unsigned long gint len = 0;
              gbool next = false;
              while(getline(&line, &len, file) > 0) {
                next = insert_chars(line); /* Note: work_saved[g] becomes false */
              }
              /* Cleanup and go*/
              fclose(file);
              if(line != NULL) free(line);
              if(next) next_gtemp();
            }
          }
          break;

        /* Show current line number */
        case '.':
          /* :.= */
          if(len_command == 3 && command[2] == '=') {
            unsigned gchar gtemp_undo = gbuffer[g].gundo;
            gchar current_line_str[32];
            gchar message[64] = "Line number: ";
            sprintf(current_line_str, "%u", gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo]);
            print(strcat(message, current_line_str);
          }

          else error("Command not recognized");
          break;

        /* Show number of lines in file */
        case '=':
          /* := */
          if(len_command == 2) {
            /* Get number of lines in file */
            unsigned gchar gtemp_undo = gbuffer[g].gundo;
            gchar total_lines_str[32];
            gchar message[64] = "Total lines: ";
            sprintf(total_lines_str, "%u", gbuffer[g].gtotal_lines[gtemp_undo]);
            print(strcat(message, total_lines_str));
          }

          else error("Command not recognized");
          break;

        /* Next open buffer */
        case 'n':
          /* :n */
          if(len_command == 2) {
            do {
              g++;
              if(g > GMAX_FILES - 1) g = 0;
            } while(!gbuffer[g].buffer_is_open);
            redraw_screen();
          }

          else error("Command not recognized");
          break;

        /* Previous open buffer */
        case 'p':
          /* :p */
          if(len_command == 2) {
            do {
              if(g == 0) g = GMAX_FILES;
              g--;
            } while(!gbuffer[g].buffer_is_open);
            redraw_screen();
          }

          else error("Command not recognized");
          break;


        /* Colon ':' command not recognized */
        default:
          error("Command not recognized");
          break;
      }


    /* Other non-colon command not recognized */
    default:
      error("Command not recognized");
      break;

  }
  return; /* For sanity; should go back to visual mode */
}

gvoid write_to_file(gchar *file_name) {
  /* Open file for writing */
  if(strlen(file_name) > 0) {
    strcpy(gbuffer[g].gfile_name, file_name);
  }
  gbuffer[g].gfile = fopen(gbuffer[g].gfile_name, 'w');
  if(gbuffer[g].gfile == NULL) {
    error("Couldn't open file for writing");
    return;
  }
  else {
    /* Else delete file for writing over and open again */
    fclose(gbuffer[g].gfile);
    unlink(gbuffer[g].gfile_name);
    gbuffer[g].gfile = fopen(gbuffer[g].gfile_name, 'w');
    if(gbuffer[g].gfile == NULL) {
      error("After opening file for writing, all data lost");
      return;
    }
  }
  
  /* Read temp file and transfer to permament file */
  unsigned gchar gtemp_undo = gbuffer[g].gundo;
  rewind(gbuffer[g].gtemp_files[gtemp_undo]);
  gchar *line = NULL;
  unsigned long gint len = 0;
  while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0) {
    fprintf(gbuffer[g].gfile, "%s", line);
  }

  /* Cleanup and go */
  gbuffer[g].work_saved = true;
  fclose(gbuffer[g].gfile);
  if(line != NULL) free(line);
  /* Close and remove temp files */
  if(gtemp_undo > 0) {
    commandmode_main(":e!");
  }
  return;
}

gvoid quit() {
  /* Cleanup temp files */
  for(unsigned gchar i=0; i<GUNDO_MAX; i++) {
    fclose(gbuffer[g].gtemp_files[i]);
    unlink(gbuffer[g].gtemp_file_names[i]);
  }
  gbuffer[g].buffer_is_open = false;

  /* Find first open buffer and redraw screen */
  unsigned gchar i=0
  for(; i<GMAX_FILES; i++) {
    if(gbuffer[g].buffer_is_open) {
      g = i;
      redraw_screen();
      break;
    }
  }
  /* Maybe end program if no open buffers */
  if(i == GMAX_FILES) {
    /* Close yank and paste buffers */
    for(unsigned gchar j=0; j<26; j++) {
      fclose(gyank[j]);
      unlink(gyank_file_names[j]);
    }
    /* Close windows */
    for(unsigned gchar j=0; j<GMAX_FILES; j++) delwin(editor_window[j]);
    delwin(command_window);
    /* End ncurses */
    endwin();
    exit(0);
  }
  return;
}
