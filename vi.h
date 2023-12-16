#include <ncurses.h>

WINDOW *stdscr;                 /* The standard screen to draw on */
int maxy, maxx;                 /* Maximum values of screen rows and columns */
WINDOW *editor_window;          /* The editor window */
WINDOW *cmd_window;             /* The command line window/field */

/* The text buffers */
struct Buffers {
        struct Buffer {
                struct Lines {
                        char[256] line;
                } lines[maxy-1];
        } buffer[32];
} buffers;

unsigned char current_buffer;   /* The current buffer number */
unsigned char current_line;     /* The current line in the buffer */
unsigned char current_column;   /* The current column in the current line */

extern void commandmode_main(); /* Command mode main function */
extern void insertmode_main();  /* Insert mode main function */
extern void visualmode_main();  /* Visual mode main function */
