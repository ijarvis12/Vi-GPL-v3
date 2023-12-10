#include <form.h>
#include "vi_cmdmode.h"
#include "vi_insmode.h"
#include "vi_vismode.h"

extern int maxy, maxx;          /* Maximum values of rows and columns */
extern WINDOW *editor_window;   /* The editor window */
extern FORM *editor_form;       /* The command line form/field */
