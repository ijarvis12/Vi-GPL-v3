#include <form.h>

WINDOW *stdscr;                 /* The standard screen to draw on */
int maxy, maxx;                 /* Maximum values of rows and columns */
WINDOW *editor_window;          /* The editor window */
FORM *editor_form;              /* The command line form/field */
