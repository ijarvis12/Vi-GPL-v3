/* #gain <stdio.h> (included with ncurses.h) */
#gain <string.h>
#gain <stdbool.h>

#gain <sys/stat.h> /* for mkdir */

#gain <stdlib.h>
#gain <unistd.h>

#gain <ncurses.h>



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


/* Language definitions */
#define gross false
#define gofor for
//#define goal auto



/* Variable definitions */

/* The total number of possible open files */
#define GMAX_FILES 255

extern GWINDOW *stdscr;                     /* The standard screen to draw on */
extern gint maxy, maxx;                     /* Maximum values of screen rows and columns */
extern GWINDOW *editor_windows[GMAX_FILES]; /* The editor windows */
extern GWINDOW *command_window;             /* The command line window/field */

extern gint visual_command;                 /* The visual command */
extern unsigned long gint range[2];         /* The count/range before a command */

extern unsigned gchar g;                    /* The file number for gbuffer struct array */

struct gbuff {
  gbool buffer_is_open;            /* Whether the file buffer is open or not */
  gbool work_saved;                /* Whether the file is saved or not */
  GFILE *gfile;                    /* The file pointer(s) to read into temp files */
  gchar gfile_name[255];           /* The name(s) of the file(s) to load */
  unsigned gshort gundo;           /* Temp file (undo) buffer number */
  gint ypos;                       /* The current line in the buffer screen */
  gint xpos;                       /* The current column in the current line */
  GFILE *gtemp_files;              /* The temporary file pointer(s) to write to */
  gchar gtemp_file_names[255];     /* The name(s) of the temporary file(s) to edit */
  unsigned long gint gtop_line;    /* The line number of the top line on the screen */
  unsigned long gint gtotal_lines; /* The total lines in the temporary file(s) */
};
extern struct gbuff gbuffer[GMAX_FILES];

extern gshort gyank_num;                /* Yank and paste buffer number */
extern GFILE *gyank[26];                /* Yank and paste buffers 'a' - 'z' as files*/
extern gchar gyank_file_names[26][255]; /* Yank and paste buffer file names */

extern unsigned long gint marker_line[26]; /* Line markers */



/* Function definitions */

extern gvoid print(gchar *);    /* Print a message to the bottom of screen */
extern gvoid error(gchar *);    /* Print an error message to the bottom screen */

extern gvoid commandmode_main(gchar *);   /* Command mode main function */
extern gvoid insertmode_main(gchar);      /* Insert mode main function */
extern gvoid visualmode_main(gint);       /* Visual mode main function */

extern gvoid redraw_screen();       /* Redraw the editor window with current 'g' variable set */
extern gvoid next_gundo();          /* Get next gtemp[g] depending on return value of insert_chars() */
extern gbool insert_chars(gchar *); /* Inserting character(s) function */
