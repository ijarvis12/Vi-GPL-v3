#include <ncurses.h>

WINDOW *stdscr;                 /* The standard screen to draw on */
int maxy, maxx;                 /* Maximum values of screen rows and columns */
WINDOW *editor_window;          /* The editor window */
WINDOW *cmd_window;             /* The command line window/field */

/* The text buffers */
struct Buffers {
        struct Buffer {
                struct Lines {
                        char[512] line;
                } lines[maxy-1];
        } buffer[32];
} buffers;

extern void commandmode_main();     /* Command mode main function */
extern void insertmode_main();     /* Insert mode main function */
extern void visualmode_main();  /* Visual mode main function */
