#include <ncurses.h>

WINDOW *stdscr;                 /* The standard screen to draw on */
int maxy, maxx;                 /* Maximum values of rows and columns */
WINDOW *editor_window;          /* The editor window */
WINDOW *cmd_window;             /* The command line window/field */
