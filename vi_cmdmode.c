#include "vi_cmdmode.h"
#include <stdlib.h>
#include <string.h>

void
commandmode_main()
{
        /* Get command from window/field */
        unsigned char command[maxx+1];
        wgetnstr(cmd_window, command, maxx); /* from ncurses */

        /* Process command */
        if(strlen(command) == 0) return; /* First a sanity check */
        unsigned char first_char = command[0];
        
        switch (first_char) {
                
                case ':':
                        /* Do rest of command */
                        if(strlen(command) == 1) break; /* First, a sanity check */
                        unsigned char second_char = command[1];
                        unsigned char len_command = strlen(command);
                        
                        switch (seocnd_char) {
                                
                                /* Write and quit */
                                case 'x':
                                        /* :x */
                                        write_to_file();
                                        quit();
                                        break;
                                
                                /* Write, maybe quit too if specified */
                                case 'w':
                                        /* :w [file] */
                                        if(len_command > 3 && command[2] == ' ') {
                                                unsigned char file_name[256];
                                                for(unsigned char i=3; i<len_command; i++) file_name[i-3] = command[i];
                                                write_to_file(file_name);
                                        }
                                        /* :w */
                                        else if(len_command == 2) write_to_file();
                                        /* :wq */
                                        else if(len_command == 3 && command[2] == 'q') {
                                                write_to_file();
                                                quit();
                                        }
                                        else error("Command not recognized");
                                        break;

                                /* Quit, maybe if everything is saved */
                                case 'q':
                                        /* :q! */
                                        if(len_command == 3 && command[2] == '!') quit();
                                        /* :q */
                                        else if(len_command == 2 && work_saved) quit();
                                        else if(len_command == 2) error("Unsaved work");

                                        else error("Command not recognized");
                                        break;

                                /* Edit, discarding changes */
                                case 'e':
                                        /* :e! */
                                        if(len_command == 3 && command[2] == '!') {
                                                /* buffer 0 throw away */
                                                /* ... */
                                                /* update screen */
                                                /* ... */
                                        }
                                        /* :e [file] */
                                        else if(len_command > 3 && command[2] == ' ') {
                                                if(work_saved) {
                                                        /* buffer 0 throw away */
                                                        /* ... */
                                                        /* open file */
                                                        /* ... */
                                                        /* update screen */
                                                        /* ... */
                                                }
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Read file in after current line */
                                case 'r':
                                        /* :r [file] */
                                        if(len_command > 3 && command[2] == ' ') {
                                                /* open file */
                                                /* ... */
                                                /* insert file after current line, buffer and screen */
                                                /* ... */
                                        }
                                        break;

                                /* Show current line number */
                                case '.':
                                        /* :.= */
                                        if(len_command == 3 && command[2] == '=') {
                                                unsigned char line_num[20]; /* 2^64 num lines possibly */
                                                itoa(current_line, line_num, 10);
                                                error("Line number: "+line_num); /* reuse code functions, right? */
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Show number of lines in file */
                                case '=':
                                        /* := */
                                        if(len_command == 2 && work_saved) {
                                                /* Get number of lines in file */
                                                /* ... */
                                                error("Total lines: ");
                                        }
                                        else if(len_command == 2) error("Work not saved");

                                        else error("Command not recognized");
                                        break;
                        }
        }
}
