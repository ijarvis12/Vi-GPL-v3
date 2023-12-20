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
                                        write_to_file("");
                                        quit();
                                        break;
                                
                                /* Write, maybe quit too if specified */
                                case 'w':
                                        /* :w [file] */
                                        if(len_command > 3 && command[2] == ' ') {
                                                unsigned char file_name[len_command-2];
                                                for(unsigned char i=3; i<len_command; i++) file_name[i-3] = command[i];
                                                write_to_file(file_name);
                                                print("File "+file_name+" saved");
                                        }
                                        /* :w */
                                        else if(len_command == 2) {
                                                write_to_file("");
                                                print("Filed saved");
                                        }
                                        /* :wq */
                                        else if(len_command == 3 && command[2] == 'q') {
                                                write_to_file("");
                                                quit();
                                        }
                                        else error("Command not recognized");
                                        break;

                                /* Quit, maybe if everything is saved */
                                case 'q':
                                        /* :q! */
                                        if(len_command == 3 && command[2] == '!') quit();
                                        /* :q */
                                        else if(len_command == 2 && work_saved[f]) quit();
                                        else if(len_command == 2) error("Unsaved work");

                                        else error("Command not recognized");
                                        break;

                                /* Edit, discarding changes */
                                case 'e':
                                        /* :e! */
                                        if(len_command == 3 && command[2] == '!') {
                                                /* Reload from permament file */
                                                files[f] = fopen(file_names[f], 'r');
                                                if(files[f] == NULL) error("Couldn't reload file");
                                                else {
                                                        /* Make a new temp file */
                                                        fclose(temp_files[f]);
                                                        remove("/var/tmp/vi/"+temp_file_names[f]);
                                                        temp_file_names[f] = tempnam("/var/tmp/vi/", NULL);
                                                        temp_files[f] = fopen("/var/tmp/vi/"+temp_file_names[f], 'w');
                                                        /* Sanity check */
                                                        if(temp_files[f] == NULL) {
                                                                error("Temp file could not be opened");
                                                                fclose(files[f]);
                                                                return;
                                                        }
                                                        /* Load permament file into temp */
                                                        char c = fgetc(files[f]);
                                                        while(c != EOF) {
                                                                fputc(c, temp_files[f]);
                                                                c = fgetc(files[f]);
                                                        }
                                                        /* Cleanup and go */
                                                        fclose(files[f]);
                                                        rewind(temp_files[f]);
                                                        work_saved[f] = true;
                                                }
                                        }
                                        /* :e [file] */
                                        else if(len_command > 3 && command[2] == ' ') {
                                                if(work_saved[f]) {
                                                        /* Load file */
                                                        char *file_save_temp;
                                                        file_save_temp = strcpy(file_save_temp, file_names[f]);
                                                        for(unsigned char i=3; i<len_command; i++) file_names[f][i-3] = command[i];
                                                        files[f] = fopen(file_names[f], 'r');
                                                        if(files[f] == NULL) {
                                                                error("Couldn't load file");
                                                                file_names[f] = file_save_temp;
                                                        }
                                                        else {
                                                                /* Make a new temp file */
                                                                fclose(temp_files[f]);
                                                                remove("/var/tmp/vi/"+temp_file_names[f]);
                                                                temp_file_names[f] = tempnam("/var/tmp/vi/", NULL);
                                                                temp_files[f] = fopen("/var/tmp/vi/"+temp_file_names[f], 'w');
                                                                /* Sanity check */
                                                                if(temp_files[f] == NULL) {
                                                                        error("Temp file could not be opened");
                                                                        fclose(files[f]);
                                                                        return;
                                                                }
                                                                /* Load permament file into temp */
                                                                char c = fgetc(files[f]);
                                                                while(c != EOF) {
                                                                        fputc(c, temp_files[f]);
                                                                        c = fgetc(files[f]);
                                                                }
                                                                /* Cleanup and go */
                                                                fclose(files[f]);
                                                                rewind(temp_files[f]);
                                                                work_saved[f] = true;
                                                        }
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
                                                char file_name[len_command-2];
                                                for(unsigned char i=3; i<len_command; i++) file_name[i-3] = command[i];
                                                FILE *file = fopen(file_name, 'r');
                                                if(file == NULL) error("Couldn't load file");
                                                else {
                                                        /* Insert file */
                                                        char c = fgetc(file);
                                                        while(c != EOF) {
                                                                insertmode_main(c); /* Note: work_saved[f] becomes false */
                                                                c = fgetc(file);
                                                        }
                                                        /* Cleanup */
                                                        fclose(file);
                                                }
                                        }
                                        break;

                                /* Show current line number */
                                case '.':
                                        /* :.= */
                                        if(len_command == 3 && command[2] == '=') {
                                                unsigned char line_num_str[10]; /* 2^32 num lines possibly */
                                                unsigned long int current_line = 0;
                                                unsigned long int temp_position = ftell(temp_files[f]);
                                                rewind(temp_files[f]);
                                                char c;
                                                for(unsigned long int i=0; i<temp_position; i++) {
                                                        c = fgetc(temp_files[f]);
                                                        if(c == '\n') current_line += 1;
                                                }
                                                itoa(current_line, line_num_str, 10);
                                                print("Line number: "+line_num_str);
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Show number of lines in file */
                                case '=':
                                        /* := */
                                        if(len_command == 2) {
                                                /* Get number of lines in file */
                                                unsigned long int temp_position = ftell(temp_files[f]);
                                                rewind(temp_files[f]);
                                                unsigned long int total_lines = 0;
                                                char c = fgetc(temp_files[f]);
                                                while(c != EOF) {
                                                        if(c == '\n') total_lines += 1;
                                                }
                                                char total_lines_str[10];
                                                itoa(total_lines, total_lines_str, 10);
                                                print("Total lines: "+total_lines_str);
                                                fseek(temp_files[f], temp_position, SEEK_SET)
                                        }

                                        else error("Command not recognized");
                                        break;
                        }
        }
        return; /* For sanity, should go back to visual mode */
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
                remove(file_names[f]);
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
        work_saved[f] = true;
        fseek(temp_files[f], temp_position, SEEK_SET);        
        fclose(files[f]);
        return;
}

void
quit()
{
        fclose(temp_files[f]);
        remove("/var/tmp/vi/"+temp_file_names[f]);
        for(unsigned char i=0; i<27; i++) {
                buffers[f].buffer[i]->lines = "";
        }
        buffer_is_open[f] = false;
        for(unsigned char i=0; i<MAX_FILES; i++) {
                if(buffer_is_open[i]) {
                        f = i;
                        break;
                }
        }
        if(i == MAX_FILES) exit(0);
        return;
}
