#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

WINDOW *stdscr;                             /* The standard screen to draw on */
int maxy, maxx;                             /* Maximum values of screen rows and columns */
WINDOW *editor_window;                      /* The editor window */
WINDOW *cmd_window;                         /* The command line window/field */

unsigned char MAX_FILES = 32;               /* The total number of possible open files */
unsigned char f;                            /* The file number, can have multiple open */
FILE *files[MAX_FILES];                     /* The file pointer(s) to read into temp files */
FILE *temp_files[MAX_FILES];                /* The temporary file pointer(s) to write to */
char *file_names[MAX_FILES];                /* The name(s) of the file(s) to load */
char *temp_file_names[MAX_FILES];           /* The names(s) of the temporary file(s) to edit */

bool work_saved[MAX_FILES];                 /* Whether the file is saved or not */

/* The text buffers */
struct Buffers {
        struct Buffer {
                struct Lines {
                        char *line;         /* The characters on a line */
                } *lines;                   /* The lines on the screen */
        } buffer[27];                       /* File buffer (0) plus buffers a-z (1-26) */
} buffers[MAX_FILES];                       /* The open buffers */

int ypos[MAX_FILES];                        /* The current line in the buffer screen */
int xpos[MAX_FILES];                        /* The current column in the current line */

void print(char *);                         /* Print a message to the bottom of screen */
void error(char *);                         /* Print an error message to the bottom screen */

extern void commandmode_main(char *);       /* Command mode main function */
extern void insertmode_main();              /* Insert mode main function */
extern void visualmode_main(unsigned char); /* Visual mode main function */
