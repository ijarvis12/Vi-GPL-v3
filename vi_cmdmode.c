#include "vi.h"
#include "vi_re.h"

gvoid write_to_file(gchar *); /* Write file to storage */
gvoid quit();                 /* Quit out of current file buffer, and maybe program */

gvoid
commandmode_main(gchar *input_command) /* Main entry point for command mode */
{
  /* Get command from window/field if no input command to calling function*/
  if(strlen(input_command) == 0) {
    unsigned gchar command[maxx+1];
    whline(command_window, ' ', maxx);
    mvwgetnstr(command_window, 0, 0, command, maxx); /* from ncurses */
  }
  else gchar *command = input_command;
  
  /* Process command */
  if(strlen(command) == 0) return; /* First a sanity check */
  
  switch (command[0]) { /* Switch on the first character */

    /* Show filename */
    case 39: /* Ctrl-g */
      if(strlen(file_names[g]) > 0) print(file_names[g]);
      else error("No filename specified");
      commandmode_main(":.=");
      commandmode_main(":=");
      unsigned long gint percent = (gtop_line[g]+ypos[g])/gtotal_lines[g];
      gchar percent_str[24] = strcat(ltoa(percent), "%");
      print(strcat(percent_str, " through file"));
      free(percent_str)
      break;

    /* Write and quit */
    case 'Z':
      /* ZZ */
      if(strlen(command) == 2 && command[1] == 'Z') {
        if(strlen(file_names[g] > 0) {
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
          if(strlen(file_names[g] > 0) {
            write_to_file("");
            quit();
          }
          else error("No filename specified");
          break;
        
        /* Write, maybe quit too if specified */
        case 'w':
          /* :w [file] */
          if(len_command > 3 && command[2] == ' ') {
            gchar file_name[len_command-2];
            for(unsigned gchar i=3; i<len_command; i++) file_name[i-3] = command[i];
            write_to_file(file_name);
            gchar message[255] = "File ";
            print(strcat(strcat(message, file_name), " saved"));
            free(file_name);
            free(message);
          }
          /* :w */
          else if(len_command == 2) {
            if(strlen(file_names[g] > 0) {
              write_to_file("");
              print("Filed saved");
            }
            else error("No filename specified");
          }
          /* :wq */
          else if(len_command == 3 && command[2] == 'q') {
            if(strlen(file_names[g] > 0) {
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
          else if(len_command == 2 && work_saved[g]) quit();
          else if(len_command == 2) error("Unsaved work");

          else error("Command not recognized");
          break;

        /* Edit */
        case 'e':
          /* :e! */
          if(len_command == 3 && command[2] == '!') {
            /* Reload from permament file */
            if(strlen(file_names[g]) > 0) files[g] = fopen(file_names[g], 'r');
            else {error("No file to reload from"); break;}
            if(files[g] == NULL) error("Couldn't reload file");
            else {
              /* Make a new temp file */
              fclose(temp_files[g]);
              remove(temp_file_names[g]);
              temp_file_names[g] = "/var/tmp/vi/";
              temp_file_names[g] = strcat(strcat(strcat(temp_file_names[g], gentenv("USER")), "/"), file_names[g]);
              temp_files[g] = fopen(temp_file_names[g], 'w');
              /* Sanity check */
              if(temp_files[g] == NULL) {
                error("Temp file could not be opened");
                fclose(files[g]);
                break; /* ****BREAK***** */
              }
              /* Load permament file into temp */
              gchar **line;
              gtotal_lines[g] = 0;
              while(getline(line, NULL, files[g]) > 0) {
                fprintf(temp_files[g], "%s", *line);
                gtotal_lines[g] += 1;
              }
              /* Cleanup and go */
              fclose(files[g]);
              rewind(temp_files[g]);
              free(line);
              work_saved[g] = true;
              gtop_line[g] = 1;
              gcurrent_pos[g] = 0;
              xpos[g] = 0;
              ypos[g] = 0;
              redraw_screen();
            }
          }
          /* :e [file] */
          else if(len_command > 3 && command[2] == ' ') {
            /* Move to next open buffer */
            unsigned gchar temp_g = g;
            do {
              g++;
              if(g > GMAX_FILES - 1) g = 0;
            } while(buffer_is_open[g] && g != temp_g);
            /* Sanity check */
            if(g == temp_g) error("No more open buffers");
            else {
              /* Maybe load file if it exists */
              for(unsigned gchar i=3; i<len_command; i++) file_names[g][i-3] = command[i];
              files[g] = fopen(file_names[g], 'r'); /* Okay if fails, usually b/c it's a new file */
              /* Make a new temp file */
              temp_file_names[g] = "/var/tmp/vi/";
              temp_file_names[g] = strcat(strcat(strcat(temp_file_names[g], gentenv("USER")), "/"), file_names[g]);
              temp_files[g] = fopen(temp_file_names[g], 'w');
              /* Sanity check */
              if(temp_files[g] == NULL) {
                error("Temp file could not be opened");
                fclose(files[g]);
                g = temp_g;
              }
              else { /* Load permament file into temp, if any to load */
                gchar **line;
                gtotal_lines[g] = 0;
                while(getline(line, NULL, files[g]) > 0) {
                  fprintf(temp_files[g], "%s", *line);
                  gtotal_lines[g] += 1;
                }
                /* Cleanup and go */
                fclose(files[g]);
                rewind(temp_files[g]);
                free(line);
                work_saved[g] = true;
                work_saved[g] = true;
                gtop_line[g] = 1;
                gcurrent_pos[g] = 0;
                xpos[g] = 0;
                ypos[g] = 0;
                redraw_screen();
              }
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
            for(unsigned gchar i=3; i<len_command; i++) file_name[i-3] = command[i];
            GFILE *file = fopen(file_name, 'r');
            if(file == NULL) error("Couldn't load file");
            else {
              /* Insert file */
              gchar **line;
              while(getline(line, NULL, file) > 0) {
                insertmode_main('i', *line); /* Note: work_saved[g] becomes false */
              }
              /* Cleanup and go*/
              fclose(file);
              free(line);
            }
            free(file_name);
          }
          break;

        /* Show current line number */
        case '.':
          /* :.= */
          if(len_command == 3 && command[2] == '=') {
            gchar current_line_str[32];
            gchar message[64] = "Line number: ";
            print(strcat(message, itoa(gtop_line[g]+ypos[g], current_line_str, 10));
            free(message);
            free(current_line_str);
          }

          else error("Command not recognized");
          break;

        /* Show number of lines in file */
        case '=':
          /* := */
          if(len_command == 2) {
            /* Get number of lines in file */
            gchar total_lines_str[32];
            gchar message[64] = "Total lines: ";
            print(strcat(message, itoa(gtotal_lines[g], total_lines_str, 10)));
            free(total_lines_str);
            free(message);
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
            } while(!buffer_is_open[g]);
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
            } while(!buffer_is_open[g]);
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
  free(input_command);
  free(command);
  return; /* For sanity; should go back to visual mode loop in vi.c */
}

gvoid
write_to_file(gchar *file_name){
  /* Open file for writing */
  if(strlen(file_name) > 0) {
    file_names[g] = file_name;
  }
  files[g] = fopen(file_names[g], 'w');
  if(files[g] == NULL) {
    error("Couldn't open file");
    return;
  }
  else {
    /* Else delete file for writing over and open again */
    fclose(files[g]);
    remove(file_names[g]);
    files[f] = fopen(file_names[g], 'w');
    if(files[g] == NULL) {
      error("After opening file, error, all data lost");
      return;
    }
  }

  /* Keep position in temp file */
  unsigned long gint temp_position = ftell(temp_files[g]); 
  
  /* Read temp file and transfer to permament file */
  rewind(temp_files[g]);
  gchar **line;
  while(getline(line, NULL, temp_files[g]) > 0) {
    fprintf(files[g], "%s", *line);
  }
  work_saved[g] = true;
  fseek(temp_files[g], temp_position, SEEK_SET);  
  fclose(files[g]);
  free(line);
  return;
}

gvoid
quit()
{
  fclose(temp_files[g]);
  remove(temp_file_names[g]);
  buffer_is_open[g] = false;
  unsigned gchar i=0
  for(; i<GMAX_FILES; i++) {
    if(buffer_is_open[i]) {
      g = i;
      redraw_screen();
      break;
    }
  }
  /* Maybe end program */
  if(i == GMAX_FILES) {
    for(unsigned gchar i=0; i<GMAX_FILES; i++) delwin(editor_window[i]);
    delwin(command_window);
    endwin();
    exit(0);
  }
  return;
}
