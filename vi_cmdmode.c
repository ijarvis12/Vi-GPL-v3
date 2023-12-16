#include "vi_cmdmode.h"
#include <string.h>

void
commandmode_main()
{
        /* Get command from window/field */
        unsigned char command[maxx+1];
        wgetnstr(cmd_window, command, maxx);

        /* Process command */
        if(strlen(command) == 0) return; /* First a sanity check */
        unsigned char first_char = command[0];
        
        switch (first_char) {
                
                case ':':
                        /* Get rest of command */
                        if(strlen(command) == 1) break; /* First, a sanity check */
                        unsigned char rest_of_command[maxx];
                        for(unsigned char i=1; i<strlen(command); i++) rest_of_command[i-1] = command[i];

                        /* Do rest of command */
                        unsigned char second_char = rest_of_command[0];
                        unsigned char len_r_of_command = strlen(rest_of_command);
                        
                        switch (seocnd_char) {
                                
                                /* Write and quit */
                                case 'x':
                                        /* :x */
                                        write_to_file();
                                        quit();
                                        break; /* in case quit() doesn't work */
                                
                                /* Write, maybe quit too if specified */
                                case 'w':
                                        /* :w [file] */
                                        if(len_r_of_command > 2 && rest_of_command[1] == ' ') {
                                                unsigned char file_name[256];
                                                for(unsigned char i=2; i<len_r_of_command; i++) file_name[i-2] = rest_of_command[i];
                                                write_to_file(file_name);
                                        }
                                        /* :w[q] */
                                        else write_to_file();
                                        /* :wq */
                                        if(len_r_of_command == 2 && rest_of_command[1] == 'q') quit();
                                        else error("Command not recognized");
                                        break;

                                /* Quit, maybe if everything is saved */
                                case 'q':
                                        /* :q! */
                                        if(len_r_of_command == 2 && rest_of_command[1] == '!') quit();
                                        else if(len_r_of_command == 1 && work_saved) quit();
                                        else if(len_r_of_command == 1) error("Unsaved work");
                                        else error("Command not recognized");
                        }
        }
}
