#include "vi_cmdmode.h"

void
commandmode_main(char *input_command)
{
        /* Get command from window/field if no input command to calling function*/
        if(strlen(input_command) == 0) {
                unsigned char command[maxx+1];
                wgetnstr(cmd_window, command, maxx); /* from ncurses */
        }
        else unsigned char command = input_command;
        
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
                                        exit(0);
                                        break;
                                
                                /* Write, maybe quit too if specified */
                                case 'w':
                                        /* :w [file] */
                                        if(len_command > 3 && command[2] == ' ') {
                                                unsigned char file_name[256];
                                                for(unsigned char i=3; i<len_command; i++) file_name[i-3] = command[i];
                                                write_to_file(file_name);
                                                work_saved[f] = true;
                                                print("File "+file_name+" saved");
                                        }
                                        /* :w */
                                        else if(len_command == 2) {
                                                write_to_file();
                                                work_saved[f] = true;
                                                print("Filed saved");
                                        }
                                        /* :wq */
                                        else if(len_command == 3 && command[2] == 'q') {
                                                write_to_file();
                                                work_saved[f] = true;
                                                exit(0);
                                        }
                                        else error("Command not recognized");
                                        break;

                                /* Quit, maybe if everything is saved */
                                case 'q':
                                        /* :q! */
                                        if(len_command == 3 && command[2] == '!') exit(0);
                                        /* :q */
                                        else if(len_command == 2 && work_saved) exit(0);
                                        else if(len_command == 2) error("Unsaved work");

                                        else error("Command not recognized");
                                        break;

                                /* Edit, discarding changes */
                                case 'e':
                                        /* :e! */
                                        if(len_command == 3 && command[2] == '!') {
                                                /* buffer 0 throw away and reload from temp file */
                                                /* ... */
                                                /* update screen */
                                                /* ... */
                                                work_saved[f] = true;
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
                                                else error("Work not saved");
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
                                                work_saved = false;
                                        }
                                        break;

                                /* Show current line number */
                                case '.':
                                        /* :.= */
                                        if(len_command == 3 && command[2] == '=') {
                                                unsigned char line_num[20]; /* 2^64 num lines possibly */
                                                itoa(current_line, line_num, 10);
                                                print("Line number: "+line_num);
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Show number of lines in file */
                                case '=':
                                        /* := */
                                        if(len_command == 2 && work_saved) {
                                                /* Get number of lines in file */
                                                /* ... */
                                                print("Total lines: ");
                                        }
                                        else if(len_command == 2) error("Work not saved");

                                        else error("Command not recognized");
                                        break;
                        }
        }
}

void
write_to_file(char *file_name){
        /* Open file for writing */
        if(strlen(file_name) > 0) {
                file_names[f] = file_name;
        }
        files[f] = fopen(file_names[f], 'w');
        if(files[f] == NULL) {
                error("Couldn't open file");
                return;
        }
        else {
                /* Else delete file for writing over and open again */
                fclose(files[f]);
                remove(files[f]);
                files[f] = fopen(file_names[f], 'w');
                if(files[f] == NULL) {
                        error("After opening file, error, all data lost");
                        return;
                }
        }

        /* Keep position in temp file */
        unsigned long int temp_position = ftell(temp_files[f]); 
        
        /* Read temp file and transfer to permament file */
        rewind(temp_files[f]);
        char c = fgetc(temp_files[f]);
        while(c != EOF) {
                fputc(c, files[f]);
                c = fgetc(temp_files[f]);
        }
        fseek(temp_files[f], temp_position, SEEK_SET);        
        fclose(files[f]);
}
