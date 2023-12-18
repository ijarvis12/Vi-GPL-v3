#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

WINDOW *stdscr;                       /* The standard screen to draw on */
int maxy, maxx;                       /* Maximum values of screen rows and columns */
WINDOW *editor_window;                /* The editor window */
WINDOW *cmd_window;                   /* The command line window/field */

unsigned char TOTAL_FILE_NUM = 32;    /* The total number of possible open files */
unsigned char f;                      /* The file number, can have multiple open */
FILE *files[TOTAL_FILE_NUM];          /* The file pointer(s) to read into buffer 0 */
char *file_names[TOTAL_FILE_NUM];     /* The name(s) of the file to edit */
long int file_poss[TOTAL_FILE_NUM];   /* The position in the file(s) */
bool work_saved[TOTAL_FILE_NUM];      /* Whether the file is saved or not */

/* The text buffers */
struct Buffers {
        struct Buffer {
                struct Lines {
                        char *line;   /* The characters on a line */
                } *lines;             /* The lines on the screen */
        } buffer[27];                 /* File buffer (0) plus buffers a-z (1-26) */
} buffers[TOTAL_FILE_NUM];            /* The open buffers */

int ypos;                             /* The current line in the buffer */
int xpos;                             /* The current column in the current line */

extern void commandmode_main(char *); /* Command mode main function */
extern void insertmode_main();        /* Insert mode main function */
extern void visualmode_main();        /* Visual mode main function */
