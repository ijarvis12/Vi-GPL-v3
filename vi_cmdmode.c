#include "vi.h"
#include "vi_re.h"

gvoid write_to_file(gchar *);    /* Write file to storage */
gvoid quit();                    /* Quit out of current file buffer, and maybe program */

gvoid
commandmode_main(gchar *input_command) /* Main entry point for command mode */
{
        /* Get command from window/field if no input command to calling function*/
        if(strlen(input_command) == 0) {
                unsigned gchar command[maxx+1];
                whline(cmd_window, ' ', maxx);
                mvwgetnstr(cmd_window, 0, 0, command, maxx); /* from ncurses */
        }
        else gchar *command = input_command;
        
        /* Process command */
        if(strlen(command) == 0) return; /* First a sanity check */
        gchar first_char = command[0];
        
        switch (first_char) {
                
                case ':':
                        /* Do rest of command */
                        if(strlen(command) == 1) break; /* First, a sanity check */
                        gchar second_char = command[1];
                        unsigned gchar len_command = strlen(command);
                        
                        switch (seocnd_char) {
                                
                                /* Write and quit */
                                case 'x':
                                        /* :x */
                                        if(strlen(file_names[g] > 0) {
                                                write_to_file("");
                                                quit();
                                        }
                                        else error("No filename specified");
                                        break;
                                
                                /* Write, maybe quit too if specified */
                                case 'w':
                                        /* :w [file] */
                                        if(len_command > 3 && command[2] == ' ') {
                                                gchar file_name[len_command-2];
                                                for(unsigned gchar i=3; i<len_command; i++) file_name[i-3] = command[i];
                                                write_to_file(file_name);
                                                print(strcat(strcat("File ",file_name)," saved"));
                                                free(file_name);
                                        }
                                        /* :w */
                                        else if(len_command == 2) {
                                                if(strlen(file_names[g] > 0) {
                                                        write_to_file("");
                                                        print("Filed saved");
                                                }
                                                else error("No filename specified");
                                        }
                                        /* :wq */
                                        else if(len_command == 3 && command[2] == 'q') {
                                                if(strlen(file_names[g] > 0) {
                                                        write_to_file("");
                                                        quit();
                                                }
                                                else error("No filename specified");
                                        }
                                                
                                        else error("Command not recognized");
                                        break;

                                /* Quit, maybe if everything is saved */
                                case 'q':
                                        /* :q! */
                                        if(len_command == 3 && command[2] == '!') quit();
                                        /* :q */
                                        else if(len_command == 2 && work_saved[g]) quit();
                                        else if(len_command == 2) error("Unsaved work");

                                        else error("Command not recognized");
                                        break;

                                /* Edit */
                                case 'e':
                                        /* :e! */
                                        if(len_command == 3 && command[2] == '!') {
                                                /* Reload from permament file */
                                                if(strlen(file_names[g]) > 0) files[g] = fopen(file_names[g], 'r');
                                                else error("No file to reload from")
                                                if(files[g] == NULL) error("Couldn't reload file");
                                                else {
                                                        /* Make a new temp file */
                                                        fclose(temp_files[g]);
                                                        remove(strcat("/var/tmp/vi/",temp_file_names[g]));
                                                        temp_file_names[g] = tempnam("/var/tmp/vi/", NULL);
                                                        temp_files[g] = fopen(strcat("/var/tmp/vi/",temp_file_names[g]), 'w');
                                                        /* Sanity check */
                                                        if(temp_files[g] == NULL) {
                                                                error("Temp file could not be opened");
                                                                fclose(files[g]);
                                                                return;
                                                        }
                                                        /* Load permament file into temp */
                                                        gchar **line;
                                                        while(getline(line, NULL, files[g]) > 0) {
                                                                fprintf(temp_files[g], "%s", *line);
                                                        }
                                                        /* Cleanup and go */
                                                        fclose(files[g]);
                                                        rewind(temp_files[g]);
                                                        free(line);
                                                        work_saved[g] = true;
                                                }
                                        }
                                        /* :e [file] */
                                        else if(len_command > 3 && command[2] == ' ') {
                                                /* Move to next open buffer */
                                                unsigned gchar temp_g = g;
                                                do {
                                                        g++;
                                                        if(g > MAX_FILES - 1) g = 0;
                                                } while(buffer_is_open[g] && g != temp_g);
                                                /* Sanity check */
                                                if(g == temp_g) error("No more open buffers");
                                                else {
                                                        /* Load file */
                                                        for(unsigned gchar i=3; i<len_command; i++) file_names[g][i-3] = command[i];
                                                        files[g] = fopen(file_names[g], 'r');
                                                        if(files[g] == NULL) {
                                                                error("Couldn't load file");
                                                                g = temp_g;
                                                        }
                                                        else {
                                                                /* Make a new temp file */
                                                                temp_file_names[g] = tempnam("/var/tmp/vi/", NULL);
                                                                temp_files[g] = fopen(strcat("/var/tmp/vi/",temp_file_names[g]), 'w');
                                                                /* Sanity check */
                                                                if(temp_files[g] == NULL) {
                                                                        error("Temp file could not be opened");
                                                                        fclose(files[g]);
                                                                        g = temp_g;
                                                                }
                                                                /* Load permament file into temp, if any to load */
                                                                gchar **line;
                                                                while(getline(line, NULL, files[g]) > 0) {
                                                                        fprintf(temp_files[g], "%s", *line);
                                                                }
                                                                /* Cleanup and go */
                                                                fclose(files[g]);
                                                                rewind(temp_files[g]);
                                                                free(line);
                                                                work_saved[g] = true;
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
                                                gchar file_name[len_command-2];
                                                for(unsigned gchar i=3; i<len_command; i++) file_name[i-3] = command[i];
                                                GFILE *file = fopen(file_name, 'r');
                                                if(file == NULL) error("Couldn't load file");
                                                else {
                                                        /* Insert file */
                                                        gchar **line;
                                                        while(getline(line, NULL, file) > 0) {
                                                                insertmode_main(*line); /* Note: work_saved[g] becomes false */
                                                        }
                                                        /* Cleanup and go*/
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
                                                unsigned gchar line_num_str[10]; /* 2^32 num lines possibly */
                                                unsigned long gint current_line = 0;
                                                unsigned long gint temp_position = ftell(temp_files[g]);
                                                rewind(temp_files[g]);
                                                gchar **line;
                                                while(getline(line, NULL, temp_files[g]) > 0 && ftell(temp_files[g]) < temp_position) {
                                                        current_line += 1;
                                                }
                                                itoa(current_line, line_num_str, 10);
                                                print(strcat("Line number: ",line_num_str));
                                                fseek(temp_files[g], temp_position, SEEK_SET);
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
                                                unsigned long gint temp_position = ftell(temp_files[g]);
                                                rewind(temp_files[g]);
                                                unsigned long gint total_lines = 0;
                                                gchar **line;
                                                while(getline(line, NULL, temp_files[g]) > 0) {
                                                        total_lines += 1;
                                                }
                                                gchar total_lines_str[10];
                                                itoa(total_lines, total_lines_str, 10);
                                                print(strcat("Total lines: ",total_lines_str));
                                                fseek(temp_files[g], temp_position, SEEK_SET);
                                                free(line);
                                                free(total_lines_str);
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Next buffer */
                                case 'n':
                                        /* :n */
                                        if(len_command == 2) {
                                                unsigned char temp_g = g;
                                                do {
                                                        g++;
                                                        if(g > MAX_FILES - 1) g = 0;
                                                } while(!buffer_is_open[g] && g != temp_g);
                                                if(g == temp_g) error("No other open buffers");
                                        }

                                        else error("Command not recognized");
                                        break;

                                /* Previous buffer */
                                case 'p':
                                        /* :p */
                                        if(len_command == 2) {
                                                unsigned char temp_g = g;
                                                do {
                                                        if(g == 0) g = MAX_FILES;
                                                        g--;
                                                } while(!buffer_is_open[g] && g != temp_g);
                                                if(g == temp_g) error("No other open buffers");
                                        }

                                        else error("Command not recognized");
                                        break;
                        }
        }
        free(input_command);
        free(command);
        return; /* For sanity, should go back to visual mode */
}

gvoid
write_to_file(gchar *file_name){
        /* Open file for writing */
        if(strlen(file_name) > 0) {
                file_names[g] = file_name;
        }
        files[g] = fopen(file_names[g], 'w');
        if(files[g] == NULL) {
                error("Couldn't open file");
                return;
        }
        else {
                /* Else delete file for writing over and open again */
                fclose(files[g]);
                remove(file_names[g]);
                files[f] = fopen(file_names[g], 'w');
                if(files[g] == NULL) {
                        error("After opening file, error, all data lost");
                        return;
                }
        }

        /* Keep position in temp file */
        unsigned long gint temp_position = ftell(temp_files[g]); 
        
        /* Read temp file and transfer to permament file */
        rewind(temp_files[g]);
        gchar **line;
        while(getline(line, NULL, temp_files[g]) > 0) {
                fprintf(files[g], "%s", *line);
        }
        work_saved[g] = true;
        fseek(temp_files[g], temp_position, SEEK_SET);        
        fclose(files[g]);
        free(line);
        return;
}

gvoid
quit()
{
        fclose(temp_files[g]);
        remove("/var/tmp/vi/"+temp_file_names[g]);
        buffer_is_open[g] = false;
        for(unsigned char i=0; i<MAX_FILES; i++) {
                if(buffer_is_open[i]) {
                        g = i;
                        break;
                }
        }
        if(i == MAX_FILES) exit(0);
        return;
}
