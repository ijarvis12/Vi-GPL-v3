#include "vi_cmdmode.h"
#include <string.h>

void
commandmode_main()
{
        /* Get command from window/field */
        char command[maxx+1];
        wgetnstr(cmd_window, command, maxx);

        /* Process command */
        char first_char = command[0];
        switch (first_char) {
                case ':':
                        /* Get rest of command */
                        char rest_command[maxx];
                        for(char i=1; i<strlen(command); i++) rest_command[i-1] = command[i];

                        /* Do rest of command */
                        char second_char = rest_command[0];
                        switch (seocnd_char) {
                                case 'x':
                        }
        }
}
