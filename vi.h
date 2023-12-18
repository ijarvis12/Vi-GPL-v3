#include <ncurses.h>

WINDOW *stdscr;                 /* The standard screen to draw on */
int maxy, maxx;                 /* Maximum values of screen rows and columns */
WINDOW *editor_window;          /* The editor window */
WINDOW *cmd_window;             /* The command line window/field */

FILE *file;                     /* The file pointer to read into buffer 0 */
char *file_name;                /* The name of the file to edit */
long int file_pos;              /* The position in the file */

/* The text buffers */
struct Buffers {
        struct Buffer {
                struct Lines {
                        char[512] line;
                } lines[maxy-1];
        } buffer[27];           /* File buffer (0) plus buffers a-z (1-26) */
} buffers;

int ypos;                       /* The current line in the buffer */
int xpos;                       /* The current column in the current line */
bool work_saved;                /* Whether the file is saved or not */

extern void commandmode_main(); /* Command mode main function */
extern void insertmode_main();  /* Insert mode main function */
extern void visualmode_main();  /* Visual mode main function */
