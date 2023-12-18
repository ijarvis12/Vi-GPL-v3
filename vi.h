#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

WINDOW *stdscr;                       /* The standard screen to draw on */
int maxy, maxx;                       /* Maximum values of screen rows and columns */
WINDOW *editor_window;                /* The editor window */
WINDOW *cmd_window;                   /* The command line window/field */

unsigned char f;                      /* The file number, can have multiple open */
FILE *files[32];                      /* The file pointer(s) to read into buffer 0 */
char *file_names[32];                 /* The name(s) of the file to edit */
long int file_poss[32];               /* The position in the file(s) */
bool work_saved[32];                  /* Whether the file is saved or not */

/* The text buffers */
struct Buffers {
        struct Buffer {
                struct Lines {
                        char *line;   /* The characters on a line */
                } *lines;             /* The lines on the screen */
        } buffer[27];                 /* File buffer (0) plus buffers a-z (1-26) */
} buffers[32];                        /* Up to 32 open files at the same time */

int ypos;                             /* The current line in the buffer */
int xpos;                             /* The current column in the current line */

extern void commandmode_main(char *); /* Command mode main function */
extern void insertmode_main();        /* Insert mode main function */
extern void visualmode_main();        /* Visual mode main function */
