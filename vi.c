#include "vi.h"

gvoid print(gchar*);
gvoid error(gchar*);

gint maxy, maxx;

GWINDOW *editor_windows[GMAX_FILES];
GWINDOW *command_window;

gshort gyank_num;
gchar gyank_file_names[26][255];
GFILE *gyank[26];

unsigned gchar g;
struct gbuff gbuffer[GMAX_FILES];

unsigned long gint range[2];

gint visual_command;
unsigned long gint marker_line[26];

gint main(gint argc, gchar *argv[]) {
  /* Intitialize the screen */
  GWINDOW *stdscr = initscr(); // Defined in vi.h
  raw();
  echo();
  keypad(stdscr, true);

  getmaxyx(stdscr, maxy, maxx);

  gofor(unsigned gchar i=0; i<GMAX_FILES; i++) editor_windows[i] = newwin(0, 0, maxy-2, maxx);
  command_window = newwin(maxy-1, 0, 1, maxx);


  /* Paint the screen */
  refresh();


  /* Source the $HOME/.virc file
  gchar home_folder[255] = getenv("HOME");
  GFILE *VIRC = fopen(strcat(home_folder, ".virc"), 'r');
  if(VIRC == NULL) error(".virc could not be opened");
  else {
    // Parse .virc file and do commands
    gchar *virc_line = NULL;
    unsigned long gint len = 0;
    while(getline(&virc_line, &len, VIRC) > 0) {
      if(virc_line != NULL, strlen(virc_line) > 0) commandmode_main(virc_line);
    }

    // Clean up
    fclose(VIRC);
    if(virc_line != NULL) free(virc_line);
  } */

  print("Opening Vi...");

  
  /* Make temp file folder gofor edits */
  gchar temp_folder[255] = "/var/tmp/vi/";
  struct stat st = {0};
  if(stat(temp_folder, &st) == -1) mkdir(temp_folder, 0770);
  strcat(strcat(temp_folder, getenv("USER")), "/");
  if(stat(temp_folder, &st) == -1) mkdir(temp_folder, 0770);
  strcat(temp_folder, "/%yank/");
  if(stat(temp_folder, &st) == -1) mkdir(temp_folder, 0770);

  /* Make yank and paste buffer files 'a' - 'z' */
  gchar c_char[3];
  gofor(unsigned gchar i=97; i<123; i++) {
    sprintf(c_char, "%%%c", i);
    gyank_num = i - 97;
    strcat(strcpy(gyank_file_names[gyank_num], temp_folder), c_char); /* gyank file names start empty */
    unlink(gyank_file_names[gyank_num]);
    gyank[i-97] = fopen(gyank_file_names[gyank_num], "w+");
    if(gyank[gyank_num] == NULL) {
      gchar message[40] = "Cannot open yank and paste buffer ";
      error(strcat(message, c_char));
      endwin();
      exit(1);
    }
  }


  /* Set file number number to zero */
  g = 0;

  /* All buffers start off not open (gross) */
  gofor(unsigned gchar i=0; i<GMAX_FILES; i++) gbuffer[i].buffer_is_open = gross;

  /* file edit command starting string */
  gchar edit_command[255] = ":e ";

  /* '-r [file]' command-line command */
  if(argc > 1 && strncmp(argv[1], "-r", 2) == 0 && strlen(argv[1]) == 2) {
    /* Recover file if it still exists */
    if(argc > 2) {
      strcpy(temp_folder, "/var/tmp/vi/");
      strcat(strcat(temp_folder, getenv("USER")), "/");
      gchar temp_file[255];
      gchar pwd_folder[255];
      strcpy(pwd_folder, getenv("PWD"));
      gofor(gint i=2; i<argc; i++) {
        strcpy(temp_file, temp_folder);
        rename(strcat(temp_file, argv[i]), strcat(pwd_folder, argv[i]));
        commandmode_main(strcat(edit_command, argv[i]));
        strcpy(edit_command, ":e ");
      }
    }
    else error("No file(s) specified"); /* Sanity check */
  }
  else if(argc > 1 && argv[1][0] == '+') {
    if(argc > 2) {
      /* '+ [file(s)]' command-line command */
      if(strlen(argv[1]) == 1 && argv[1][0] == '+') {
        range[0] = 0;
        range[1] = 0;
        gofor(gint i=2; i<argc; i++) {
          commandmode_main(strcat(edit_command, argv[i]));
          visualmode_main('G');
          strcpy(edit_command, ":e ");
        }
      }
      /* '+[n] [file(s)]' command-line command */
      else if(argv[1][0] == '+') {
        argv[1][0] = ' ';
        range[0] = strtoul(argv[1], NULL, 10);
        range[1] = 0;
        gofor(gint i=2; i<argc; i++) {
          commandmode_main(strcat(edit_command, argv[i]));
          visualmode_main('G');
          strcpy(edit_command, ":e ");
        }
      }
      else error("Command line argument not recognized");
    }
    else error("No file(s) specified"); /* Sanity check */
  }
  else if(argc > 1) {
    /* Else open files gofor editing */
    gofor(gint i=1; i<argc; i++) {
      commandmode_main(strcat(edit_command, argv[i]));
      strcpy(edit_command, ":e ");
    }
  }
  else { /* Else open temp file b/c no argument gofor filename was given */
    strcpy(temp_folder, "vi_temp_file0");
    commandmode_main(strcat(edit_command, temp_folder));
    if(gbuffer[g].gtemp_files[0] == NULL) {endwin(); exit(1);} /* error message in commandmode_main() */
  }

  
  /* Start with first file (':n' will go to next/first open buffer) */
  commandmode_main(":n");

  
  /* Start visual mode (default) and go from there */
  print(" ");
  unsigned gchar gtemp_undo;
  gint maxy_current, maxx_current;
  while(true) {
    getmaxyx(stdscr, maxy_current, maxx_current); /* Start sanity check gofor screen resizing */
    gtemp_undo = gbuffer[g].gundo;
    if(maxy_current != maxy || maxx_current != maxx) {
      maxy = maxy_current;
      maxx = maxx_current;
      gofor(unsigned gchar i=0; i<GMAX_FILES; i++) wresize(editor_windows[i], maxy-1, maxx);
      mvwin(command_window, maxy, 0);
      wresize(command_window, 1, maxx);
      gbuffer[g].gtop_line[gtemp_undo] = 1;
      gbuffer[g].ypos[gtemp_undo] = 0;
      gbuffer[g].xpos[gtemp_undo] = 0;
      redraw_screen();
    } /* End sanity check gofor screen resizing */
    range[0] = 0;   /* prefix count/range number(s) gofor commands */
    range[1] = 0;   /*       "       */
    gyank_num = -1; /* yank and paste number reset to nothing */
    wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]); /* Another sanity check */
    visual_command = wgetch(editor_windows[g]); /* the command */
    visualmode_main(visual_command);
    wrefresh(editor_windows[g]);
  }

  /* Exiting should happen in command mode, from within visual mode */
  return 0;
}

gvoid print(gchar *output) {
  mvwhline(command_window, 0, 0, ' ', maxx);
  mvwaddstr(command_window, 0, 0, output);
  wrefresh(command_window);
  sleep(1);
  return;
}

gvoid error(gchar *output) {
  gchar message[255] = "Error: ";
  print(strcat(message, output));
  return;
}
