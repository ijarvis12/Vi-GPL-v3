#include "vi.h"
#include "re.h"

void commandmode_main(char *); /* Main entry point for command mode */
void write_to_file(char *);    /* Write file to storage */
void quit();                   /* Quit out of current file buffer, and maybe program */
