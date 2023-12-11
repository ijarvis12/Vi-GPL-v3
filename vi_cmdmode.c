#include "vi_cmdmode.h"

void
cmdmode_main()
{
        /* Get command from window/field */
        char command[maxx+1];
        wgetnstr(cmd_window, command, maxx);

        /* Process command */
        char first_char = command[0];
        switch (first_char) {

        }
}
