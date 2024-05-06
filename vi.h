/* #include <stdio.h> (included with ncurses.h) */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/stat.h> /* for mkdir */

#include <ncurses.h>

#include <unistd.h> /* Need this after ncurses.h for move() */


/* Type definitions */
#define gvoid void
#define gbool bool
#define gchar char
#define gshort short
#define gint int
#define gfloat float
#define gdouble double

#define GWINDOW WINDOW
#define GFILE FILE

/* The total number of possible open files */
#define GMAX_FILES 255
/* Maximum number of undo's */
#define GUNDO_MAX 9

extern GWINDOW *stdscr;                     /* The standard screen to draw on */
extern gint maxy, maxx;                     /* Maximum values of screen rows and columns */
extern GWINDOW *editor_windows[GMAX_FILES]; /* The editor windows */
extern GWINDOW *command_window;             /* The command line window/field */

extern gint visual_command;                 /* The visual command */
extern unsigned long gint range[2];         /* The count/range before a command */

extern unsigned gchar g;                    /* The file number for gbuffer struct array */

struct gbuff {
  gbool buffer_is_open;              /* Whether the file buffer is open or not */
  gbool work_saved;                  /* Whether the file is saved or not */
  GFILE *gfile;                      /* The file pointer(s) to read into temp files */
  gchar gfile_name[255];             /* The name(s) of the file(s) to load */
  unsigned gchar gundo;              /* Temp file (undo) buffer number */
  gint ypos[GUNDO_MAX];              /* The current line in the buffer screen */
  gint xpos[GUNDO_MAX];              /* The current column in the current line */
  GFILE *gtemp_files[GUNDO_MAX];     /* The temporary file pointer(s) to write to */
  gchar gtemp_file_names[GUNDO_MAX][255];     /* The name(s) of the temporary file(s) to edit */
  unsigned long gint gtop_line[GUNDO_MAX];    /* The line number of the top line on the screen */
  unsigned long gint gtotal_lines[GUNDO_MAX]; /* The total lines in the temporary file(s) */
};
extern struct gbuff gbuffer[GMAX_FILES];

extern gshort gyank_num;                /* Yank and paste buffer number */
extern GFILE *gyank[26];                /* Yank and paste buffers 'a' - 'z' as files*/
extern gchar gyank_file_names[26][255]; /* Yank and paste buffer file names */

extern unsigned long gint marker_line[26]; /* Line markers */

extern gvoid print(gchar *);    /* Print a message to the bottom of screen */
extern gvoid error(gchar *);    /* Print an error message to the bottom screen */

extern gvoid commandmode_main(gchar *);   /* Command mode main function */
extern gvoid insertmode_main(gchar);      /* Insert mode main function */
extern gvoid visualmode_main(gint);       /* Visual mode main function */

extern gvoid redraw_screen();       /* Redraw the editor window with current 'g' variable set */
extern gvoid next_gtemp();          /* Get next gtemp[g] depending on return value of insert_chars() */
extern gbool insert_chars(gchar *); /* Inserting character(s) function */
