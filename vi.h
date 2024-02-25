/* #include <stdio.h> (included with ncurses.h) */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>

/* Type definitions */
typedef void gvoid;
typedef bool gbool;
typedef char gchar;
typedef char* gchar*;
typedef char** gchar**;
typedef unsigned char unsigned gchar; 
typedef short gshort;
typedef unsigned short unsigned gshort;
typedef int gint;
typedef int* gint*;
typedef unsigned int unsigned gint;
typedef unsigned int* unsigned gint*;
typedef float gfloat;
typedef long int long gint;
typedef unsigned long int unsigned long gint;
typedef double gdouble;
typedef WINDOW* GWINDOW*;
typedef WINDOW** GWINDOW**;
typedef FILE* GFILE*;
typedef FILE** GFILE**;


unsigned gchar GMAX_FILES = 255;     /* The total number of possible open files */
unsigned gchar GUNDO_MAX = 9;        /* Maximum number of undo's */

GWINDOW *stdscr;                     /* The standard screen to draw on */
gint maxy, maxx;                     /* Maximum values of screen rows and columns */
GWINDOW *editor_windows[GMAX_FILES]; /* The editor windows */
GWINDOW *command_window;             /* The command line window/field */

unsigned gint ypos[GMAX_FILES];    /* The current line in the buffer screen */
unsigned gint xpos[GMAX_FILES];    /* The current column in the current line */

gint visual_command;                /* The visual command */
unsigned long gint range[2];        /* The count/range before a command */

unsigned gchar g;                              /* The file number, can have multiple open */
unsigned gchar gtemp[GMAX_FILES];              /* Temp file (undo) buffer number */
GFILE *files[GMAX_FILES];                      /* The file pointer(s) to read into temp files */
GFILE *temp_files[GMAX_FILES][GUNDO_MAX];      /* The temporary file pointer(s) to write to */
gchar *file_names[GMAX_FILES];                 /* The name(s) of the file(s) to load */
gchar *temp_file_names[GMAX_FILES][GUNDO_MAX}; /* The name(s) of the temporary file(s) to edit */

gbool buffer_is_open[GMAX_FILES];   /* Whether the file buffer is open or not */
gbool work_saved[GMAX_FILES];       /* Whether the file is saved or not */

unsigned long gint gtop_line[GMAX_FILES][GUNDO_MAX];    /* The line number of the top line on the screen */
unsigned long gint gcurrent_pos[GMAX_FILES];            /* The current position in the temporary file(s) */
unsigned long gint gtotal_lines[GMAX_FILES][GUNDO_MAX]; /* The total lines in the temporary file(s) */

unsigned gchar gyank_num;     /* Yank and paste buffer number */
GFILE *gyank[26];            /* Yank and paste buffers 'a' - 'z' as files*/
gchar *gyank_file_names[26]; /* Yank and paste buffer file names */

gvoid print(gchar *);    /* Print a message to the bottom of screen */
gvoid error(gchar *);    /* Print an error message to the bottom screen */

extern gvoid commandmode_main(gchar *);       /* Command mode main function */
extern gvoid insertmode_main(gchar, gchar *); /* Insert mode main function */
extern gvoid visualmode_main(gint);           /* Visual mode main function */

extern gvoid redraw_screen();       /* Redraw the editor window with current 'g' variable set */
extern gvoid next_gtemp();          /* Get next gtemp[g] depending on return value of insert_chars() */
extern gbool insert_chars(gchar *); /* Inserting character(s) function */
