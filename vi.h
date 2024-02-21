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

GWINDOW *stdscr;                     /* The standard screen to draw on */
gint maxy, maxx;                     /* Maximum values of screen rows and columns */
GWINDOW *editor_windows[GMAX_FILES]; /* The editor windows */
GWINDOW *command_window;             /* The command line window/field */

gint visual_command;               /* The visual command */
unsigned gint range[2];            /* The count/range before a command */

unsigned gchar g;                   /* The file number, can have multiple open */
GFILE *files[GMAX_FILES];           /* The file pointer(s) to read into temp files */
GFILE *temp_files[GMAX_FILES];      /* The temporary file pointer(s) to write to */
gchar *file_names[GMAX_FILES];      /* The name(s) of the file(s) to load */
gchar *temp_file_names[GMAX_FILES]; /* The name(s) of the temporary file(s) to edit */

unsigned long gint gtop_line[GMAX_FILES];    /* The line number of the top line on the screen */
unsigned long gint gcurrent_pos[GMAX_FILES]; /* The current position in the temporary file */

unsigned gchar ascii_buffer_number; /* Yank and paste buffer number */
gchar *buffers[27];                 /* Yank and paste buffers 'a' - 'z' plus a default zero */
gbool buffer_is_open[GMAX_FILES];   /* Whether the file buffer is open or not */
gbool work_saved[MAX_FILES];        /* Whether the file is saved or not */

unsigned gint ypos[GMAX_FILES];    /* The current line in the buffer screen */
unsigned gint xpos[GMAX_FILES];    /* The current column in the current line */

gvoid print(gchar *);    /* Print a message to the bottom of screen */
gvoid error(gchar *);    /* Print an error message to the bottom screen */

extern gvoid commandmode_main(gchar *);       /* Command mode main function */
extern gvoid insertmode_main(gchar, gchar *); /* Insert mode main function */
extern gvoid visualmode_main(gint);           /* Visual mode main function */

extern gvoid redraw_screen();    /* Redraw the editor window with current 'g' variable set */
