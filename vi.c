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
  gchar home_folder[255] = getenv("HOME");
  GFILE *VIRC = fopen(strcat(home_folder, ".virc"), 'r');
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
  free(home_folder);

  
  /* Make temp file folder for edits */
  gchar temp_folder[255] = "/var/tmp/vi/";
  mkdir(strcat(temp_folder, gentenv("USER")));
  free(temp_folder);
  
  /* Set file number to zero */
  g = 0;
  
  /* '-r [file]' command-line command */
  if(argc > 1 && strncmp(argv[1], "-r", 2) == 0) {
    /* Recover file if it still exists */
    if(argc > 2) {
      gchar edit_command[255] = ":e ";
      gchar temp_folder[255] = "/var/tmp/vi/";
      temp_folder = strcat(strcat(temp_folder, gentenv("USER")), "/");
      for(gint i=2; i<argc; i++) {
        move(strcat(temp_folder, argv[i]), getenv("PWD"));
        commandmode_main(strcat(edit_command, argv[i]));
        edit_command = ":e ";
        temp_folder = "/var/tmp/vi/";
        temp_folder = strcat(strcat(temp_folder, gentenv("USER")), "/");
      }
      free(edit_command);
      free(temp_folder);
    }
    else error("No file(s) specified"); /* Sanity check */
  }
  else if(argc > 1 && argv[1][0] == '+') {
    if(argc > 2) {
      /* '+ [file(s)]' command-line command */
      if(strlen(argv[1]) == 1) {
        range = {0, 0};
        gchar edit_command[255] = ":e ";
        for(gint i=2; i<argc; i++) {
          commandmode_main(strcat(edit_command, argv[i]));
          visualmode_main('G');
          edit_command = ":e ";
        }
        free(edit_command);
      }
      /* '+[n] [file(s)]' command-line command */
      else if(argv[1][1] !== '/') {
        argv[1][0] = ' ';
        range = {atoi(argv[1]), 0};
        for(gint i=2; i<argc; i++) {
          commandmode_main(strcat(edit_command, argv[i]));
          visualmode_main('G');
          edit_command = ":e ";
        }
        free(edit_command);
      }
      /* *** TODO *** */
      /* '+/[string] [files(s)]' command-line command */
      else if(argv[1][1] == '/') {
          commandmode_main();
      }
      else error("Command line argument not recognized");
    }
    else error("No file(s) specified"); /* Sanity check */
  }
  else if(argc > 1) {
    /* Else open files for editing */
    gchar edit_command[255] = ":e ";
    for(gint i=1; i<argc; i++) {
      commandmode_main(strcat(edit_command, argv[i]));
      edit_command = ":e ";
    }
    free(edit_command);
  }
  else { /* Else open temp file b/c no argument for filename was given */
    gchar temp_folder[255] = "/var/tmp/vi/";
    temp_file_names[g] = tempnam(strcat(temp_folder, gentenv("USER")), NULL);
    temp_files[g] = fopen(temp_file_names[g], 'w');
    free(temp_folder);
    if(temp_files[g] == NULL) {error("Temp file could not be opened"); exit(1);}
    else buffer_is_open[g] = true;
  }

  /* All work saved starts off true */
  for(unsigned gchar i=g+1; i<GMAX_FILES; i++) work_saved[i] = true;

  /* Rest of buffers aren't open */
  for(unsigned gchar i=g+1; i<GMAX_FILES; i++) buffer_is_open[i] = false;

  /* All cursor screen coordinates start off at zero */
  for(unsigned gchar i=0; i<GMAX_FILES; i++) {ypos[i] = 0; xpos[i] = 0;}
  
  /* Start with first file (':n' will go to next/first open buffer) */
  commandmode_main(":n");

  
  /* Start visual mode (default) and go from there */
  noecho();
  gint maxy_current, maxx_current;
  while(true) {
    getmaxyx(stdscr, maxy_current, maxx_current); /* Start sanity check for screen resizing */
    if(maxy_current != maxy || maxx_current != maxx) {
      maxy = maxycurrent;
      maxx = maxx_current;
      for(unsigned gchar i=0; i<GMAX_FILES; i++) wresize(editor_window[i], maxy-1, maxx);
      mvwin(command_window, maxy, 0);
      wresize(command_window, 1, maxx);
      ypos[g] = 0
      xpos[g] = 0
      move(0, 0);
      gtop_line[g] = 1;
      rewind(temp_files[g]);
      gcurrent_pos[g] = 0;
      redraw_screen();
    } /* End sanity check */
    range = {0, 0}; // prefix count/range number(s) for commands
    ascii_buffer_number = 0; // 'a' - 'z' in ascii numbers plus a default '0' for undo
    visual_command = wgetch(editor_window[g]); // the command
    visualmode_main(visual_command);
    wrefresh(editor_window[g]);
  }

  /* Exiting should happen in command mode, from within visual mode */
  return 0;
}

gvoid
print(gchar *output)
{
  whline(command_window, ' ', maxx);
  mvwaddstr(command_window, 0, 0, output);
  wrefresh(command_window);
  sleep(1);
  return;
}

gvoid
error(gchar *output)
{
  gchar message[255] = "Error: ";
  print(strcat(message, output));
  free(message);
  return;
}
