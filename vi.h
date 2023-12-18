#include <ncurses.h>

WINDOW *stdscr;                 /* The standard screen to draw on */
int maxy, maxx;                 /* Maximum values of screen rows and columns */
WINDOW *editor_window;          /* The editor window */
WINDOW *cmd_window;             /* The command line window/field */

unsigned char f;                /* The file number, can have multiple open */
FILE *file[32];                 /* The file pointer(s) to read into buffer 0 */
char *file_name[32];            /* The name(s) of the file to edit */
long int file_pos[32];          /* The position in the file(s) */

/* The text buffers */
struct Buffers {
        struct Buffer {
                struct Lines {
                        char *line;
                } *lines;
        } buffer[27];           /* File buffer (0) plus buffers a-z (1-26) */
} buffers[32];                  /* Up to 32 open files at the same time */

int ypos;                       /* The current line in the buffer */
int xpos;                       /* The current column in the current line */
bool work_saved;                /* Whether the file is saved or not */

extern void commandmode_main(); /* Command mode main function */
extern void insertmode_main();  /* Insert mode main function */
extern void visualmode_main();  /* Visual mode main function */
