#include "vi.h"

gint
main(gint argc, gchar *argv[])
{
  /* Intitialize the screen */
  stdscr = initscr(); // Defined in vi.h
  raw();
  echo();
  keypad(stdscr, TRUE);

  getmaxyx(stdscr, maxy, maxx); // maxy, maxx defined in vi.h

  for(unsigned gchar i=0; i<GMAX_FILES; i++) {
    editor_window[i] = newwin(0, 0, maxy-1, maxx); // Defined in vi.h
  }
  commmand_window = newwin(maxy, 0, 1, maxx); // Defined in vi.h


  /* Paint the screen */
  refresh();


  /* Source the $HOME/.virc file */
  GFILE *VIRC = fopen("$HOME/.virc", 'r');
  if(VIRC == NULL) error(".virc could not be opened");
  else {
    // Parse .virc file and do commands
    gchar **virc_line;
    while(getline(virc_line, NULL ,VIRC) > 0) {
      if(strlen(*virc_line) > 0) commandmode_main(*virc_line);
    }

    // Clean up
    fclose(VIRC);
    free(virc_line);
  }

  
  /* Make temp file folder for edits */
  mkdir("/var/tmp/vi");
  
  /* Set file number to zero */
  g = 0;
  
  /* '-r [file]' command-line command */
  if(strncmp(argv[1], "-r ", 3) == 0){
    /* Recover file if it still exists */
    if(argc > 2 && argc < 259) {
      gchar edit_command[256] = ":e ";
      for(gshort i=2; i<argc; i++) {
        commandmode_main(strcat(strcat(edit_command, "/var/tmp/vi/"), argv[i]));
        edit_command = ":e ";
      }
      free(edit_command);
    }
  }
  else {
    /* Else open the file(s) */
    for(gint i=1; i<argc; i++) {
      if(g > GMAX_FILES - 1) {error("Too many files specified"); break;} /* Sanity check */
      file_names[g] = argv[i];
      files[g] = fopen(file_names[f], 'r'); /* Dont' care if fails, could be new file */

      /* Open temp file for edits */
      temp_file_names[g] = tempnam("/var/tmp/vi", NULL);
      temp_files[g] = fopen(strcat("/var/tmp/vi/",temp_file_names[g]), 'w');
      if(temp_files[g] == NULL) {
        error("Temp file could not be opened");
        return 1;
      }

      /* Pull in file contents if appropriate */
      else if(files[g] != NULL) {
        gchar **line;
        while(getline(line, NULL , files[g]) > 0) {
          fprintf(temp_files[g], "%s", *line);
        }
        rewind(temp_files[g]);
        fclose(files[g]);
        free(line);
        buffer_is_open[g] = true;
      }

      /* Increment file number */
      g++;
    }
  }


  /* All work saved starts off true */
  for(unsigned gchar i=0; i<GMAX_FILES; i++) work_saved[i] = true;

  /* Rest of buffers aren't open */
  for(unsigned gchar i=g; i<GMAX_FILES; i++) buffer_is_open[i] = false;

  /* All cursor screen coordinates start off at zero */
  for(unsigned gchar i=0; i<GMAX_FILES; i++) {ypos[i] = 0; xpos[i] = 0;}
  
  /* Open temp file if no argument for filename was given */
  if(g == 0) {
    temp_file_names[g] = tempnam("/var/tmp/vi", NULL);
    temp_files[g] = fopen(strcat("/var/tmp/vi/",temp_file_names[g]), 'w');
    if(temp_files[g] == NULL) {error("Temp file could not be opened"); return 1;}
    else buffer_is_open[g] = true;
  }
  else g = 0;

  
  /* Start visual mode (default) and go from there */
  noecho();
  while(true) {
    wgetyx(editor_window[g], ypos[g], xpos[g]);
    visual_command = wgetch(editor_window[g]); // the command
    count[2] = {0, 0}; // prefix number(s) for commands
    ascii_buffer_number = 0; // 'a' - 'z' in ascii numbers plus a default '0' for undo
    visualmode_main(visual_command);
  }

  /* Exiting happens in command mode, from within visual mode */
  return 0;
}

gvoid
print(gchar *output)
{
  whline(command_window, ' ', maxx);
  mvwaddstr(command_window, 0, 0, output);
  refresh();
  sleep(1);
}

gvoid
error(gchar *output)
{
  print(strcat("Error: ",output));
}
