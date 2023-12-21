#include "vi.h"
#include "re.h"

void write_to_file(char *);    /* Write file to storage */
void quit();                   /* Quit out of current file buffer, and maybe program */

void
commandmode_main(char *input_command) /* Main entry point for command mode */
{
        /* Get command from window/field if no input command to calling function*/
        if(strlen(input_command) == 0) {
                unsigned char command[maxx+1];
                wgetnstr(cmd_window, command, maxx); /* from ncurses */
        }
        else char *command = input_command;
        
        /* Process command */
        if(strlen(command) == 0) return; /* First a sanity check */
        char first_char = command[0];
        
        switch (first_char) {
                
                case ':':
                        /* Do rest of command */
                        if(strlen(command) == 1) break; /* First, a sanity check */
                        char second_char = command[1];
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
                                                char file_name[len_command-2];
                                                for(unsigned char i=3; i<len_command; i++) file_name[i-3] = command[i];
                                                write_to_file(file_name);
                                                print("File "+file_name+" saved");
                                                free(file_name);
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

                                /* Edit */
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
                                                        char **line;
                                                        while(getline(line, NULL, files[f]) > 0) {
                                                                fprintf(temp_files[f], "%s", *line);
                                                        }
                                                        /* Cleanup and go */
                                                        fclose(files[f]);
                                                        rewind(temp_files[f]);
                                                        free(line);
                                                        work_saved[f] = true;
                                                }
                                        }
                                        /* :e [file] */
                                        else if(len_command > 3 && command[2] == ' ') {
                                                /* Move to next open buffer */
                                                unsigned char temp_f = f;
                                                do {
                                                        f++;
                                                        if(f > MAX_FILES - 1) f = 0;
                                                } while(buffer_is_open[f] && f != temp_f);
                                                /* Sanity check */
                                                if(f == temp_f) error("No more open buffers");
                                                else {
                                                        /* Load file */
                                                        for(unsigned char i=3; i<len_command; i++) file_names[f][i-3] = command[i];
                                                        files[f] = fopen(file_names[f], 'r');
                                                        if(files[f] == NULL) {
                                                                error("Couldn't load file");
                                                                f = temp_f;
                                                        }
                                                        else {
                                                                /* Make a new temp file */
                                                                temp_file_names[f] = tempnam("/var/tmp/vi/", NULL);
                                                                temp_files[f] = fopen("/var/tmp/vi/"+temp_file_names[f], 'w');
                                                                /* Sanity check */
                                                                if(temp_files[f] == NULL) {
                                                                        error("Temp file could not be opened");
                                                                        fclose(files[f]);
                                                                        f = temp_f;
                                                                }
                                                                /* Load permament file into temp */
                                                                char **line;
                                                                while(getline(line, NULL, files[f]) > 0) {
                                                                        fprintf(temp_files[f], "%s", *line);
                                                                }
                                                                /* Cleanup and go */
                                                                fclose(files[f]);
                                                                rewind(temp_files[f]);
                                                                free(line);
                                                                work_saved[f] = true;
                                                        }
                                                }
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
                                                        char **line;
                                                        while(getline(line, NULL, file) > 0) {
                                                                insertmode_main(*line); /* Note: work_saved[f] becomes false */
                                                        }
                                                        /* Cleanup */
                                                        fclose(file);
                                                        free(line);
                                                }
                                                free(file_name);
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
                                                char **line;
                                                while(getline(line, NULL, temp_files[f]) > 0 && ftell(temp_files[f]) < temp_position) {
                                                        current_line += 1;
                                                }
                                                itoa(current_line, line_num_str, 10);
                                                print("Line number: "+line_num_str);
                                                fseek(temp_files[f], temp_position, SEEK_SET);
                                                free(line);
                                                free(line_num_str);
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
                                                char **line;
                                                while(getline(line, NULL, temp_files[f]) > 0) {
                                                        total_lines += 1;
                                                }
                                                char total_lines_str[10];
                                                itoa(total_lines, total_lines_str, 10);
                                                print("Total lines: "+total_lines_str);
                                                fseek(temp_files[f], temp_position, SEEK_SET);
                                                free(line);
                                                free(total_lines_str);
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Next buffer */
                                case 'n':
                                        /* :n */
                                        if(len_command == 2) {
                                                unsigned char temp_f = f;
                                                do {
                                                        f++;
                                                        if(f > MAX_FILES - 1) f = 0;
                                                } while(!buffer_is_open[f] && f != temp_f);
                                                if(f == temp_f) error("No other open buffers");
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Previous buffer */
                                case 'p':
                                        /* :p */
                                        if(len_command == 2) {
                                                unsigned char temp_f = f;
                                                do {
                                                        if(f == 0) f = MAX_FILES;
                                                        f--;
                                                } while(!buffer_is_open[f] && f != temp_f);
                                                if(f == temp_f) error("No other open buffers");
                                        }

                                        else error("Command not recognized");
                                        break;
                        }
        }
        free(input_command);
        free(command);
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
        char **line;
        while(getline(line, NULL, temp_files[f]) > 0) {
                fprintf(files[f], "%s", *line);
        }
        work_saved[f] = true;
        fseek(temp_files[f], temp_position, SEEK_SET);        
        fclose(files[f]);
        free(line);
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
