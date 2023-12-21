#include "vi.h"

int
main(int argc, char *argv[])
{
        /* Intitialize the screen */
        stdscr = initscr(); // Defined in vi.h
        raw();
        echo();
        keypad(stdscr, TRUE);

        getmaxyx(stdscr, maxy, maxx); // maxy, maxx defined in vi.h

        for(unsigned char i=0; i<MAX_FILES; i++) {
                editor_window[i] = newwin(0, 0, maxy-1, maxx); // Defined in vi.h
        }
        commmand_window = newwin(maxy, 0, 1, maxx); // Defined in vi.h


        /* Paint the screen */
        refresh();


        /* Source the $HOME/.virc file */
        FILE *VIRC = fopen("$HOME/.virc", 'r');
        if(VIRC == NULL) error(".virc could not be opened");
        else {
                // Parse .virc file and do commands
                char **virc_line;
                while(getline(virc_line, NULL ,VIRC) > 0) {
                        if(strlen(*virc_line) > 0) commandmode_main(*virc_line);
                }

                // Clean up
                fclose(VIRC);
                free(virc_line);
        }

        
        /* Set extern vars from cmdline opts using argc, argv */
        /* And open temp files for edits */
        mkdir("/var/tmp/vi");
        f = 0;
        for(int i=1; i<argc; i++) {
                if(argv[i][0] == '-') {
                        /* Get command and execute */
                        char command[strlen(argv[i])+1];
                        command[0] = ':';
                        for(unsigned char j=1; j<strlen(argv[i]); j++) command[j] = argv[i][j];
                        char *hyphen = strchr(command, '-');
                        command[*hypen] = ' ';
                        commandmode_main(command);
                }
                else {
                        /* Open the file(s) */
                        if(f > MAX_FILES - 1) {error("Too many files specified"); break;} /* Sanity check */
                        file_names[f] = argv[i];
                        files[f] = fopen(file_names[f], 'r'); /* Dont' care if fails, could be new file */

                        /* Open temp file for edits */
                        temp_file_names[f] = tempnam("/var/tmp/vi", NULL);
                        temp_files[f] = fopen("/var/tmp/vi/"+temp_file_names[f], 'w');
                        if(temp_files[f] == NULL) {
                                error("Temp file could not be opened");
                                return 1;
                        }

                        /* Pull in file contents if appropriate */
                        else if(files[f] != NULL) {
                                char **line;
                                while(getline(line, NULL , files[f]) > 0) {
                                        fprintf(temp_files[f], "%S", *line);
                                }
                                rewind(temp_files[f]);
                                fclose(files[f]);
                                free(line);
                                buffer_is_open[f] = true;
                        }

                        /* Increment file number */
                        f++;
                }
        }


        /* All work saved starts off true */
        for(unsigned char i=0; i<MAX_FILES; i++) work_saved[i] = true;

        /* Rest of buffers aren't open */
        for(unsigned char i=f; f<MAX_FILES; i++) buffer_is_open[i] = false;
        
        /* Open temp file if no argument for filename was given */
        if(f == 0) {
                temp_file_names[f] = tempnam("/var/tmp/vi", NULL);
                temp_files[f] = fopen("/var/tmp/vi/"+temp_file_names[f], 'w');
                if(temp_files[f] == NULL) {error("Temp file could not be opened"); return 1;}
                else buffer_is_open[f] = true;
        }
        else f = 0;

        
        /* Start visual mode (default) and go from there */
        visualmode_main();


        /* Done with program, close temp files, free memory */
        for(unsigned char i=0; i<MAX_FILES; i++) {
                fclose(temp_files[i]);
                remove("/var/temp/vi/"+temp_file_names[i]);
        }
        delwin(editor_window);
        delwin(command_window);

        /* End program */
        endwin();
        return 0;
}

void
print(char *output)
{
        whline(command_window, ' ', maxx);
        waddstr(command_window, output);
        refresh();
        sleep(1);
}

void
error(char *output)
{
        print("Error: "+output);
}
