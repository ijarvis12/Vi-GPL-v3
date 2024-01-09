/* #include <stdio.h> (Included with ncurses.h) */
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
typedef unsigned int unsigned gint;
typedef float gfloat;
typedef long int long gint;
typedef unsigned long int unsigned long gint;
typedef double gdouble;
typedef WINDOW* GWINDOW*;
typedef FILE* GFILE*;


unsigned gchar MAX_FILES = 32;               /* The total number of possible open files */

GWINDOW *stdscr;                             /* The standard screen to draw on */
gint maxy, maxx;                             /* Maximum values of screen rows and columns */
GWINDOW *editor_windows[MAX_FILES];          /* The editor windows */
GWINDOW *command_window;                     /* The command line window/field */

unsigned gchar g;                            /* The file number, can have multiple open */
GFILE *files[MAX_FILES];                     /* The file pointer(s) to read into temp files */
GFILE *temp_files[MAX_FILES];                /* The temporary file pointer(s) to write to */
gchar *file_names[MAX_FILES];                /* The name(s) of the file(s) to load */
gchar *temp_file_names[MAX_FILES];           /* The names(s) of the temporary file(s) to edit */

gbool buffer_is_open[MAX_FILES];             /* Whether the buffer is open or not */
gbool work_saved[MAX_FILES];                 /* Whether the file is saved or not */

gint ypos[MAX_FILES];                        /* The current line in the buffer screen */
gint xpos[MAX_FILES];                        /* The current column in the current line */

gvoid print(gchar *);                        /* Print a message to the bottom of screen */
gvoid error(gchar *);                        /* Print an error message to the bottom screen */

extern gvoid commandmode_main(gchar *);      /* Command mode main function */
extern gvoid insertmode_main(gchar *);       /* Insert mode main function */
extern gvoid visualmode_main();              /* Visual mode main function */
