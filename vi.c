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

        editor_window = newwin(0, 0, maxy-1, maxx); // Defined in vi.h
        cmd_window = newwin(maxy, 0, 1, maxx); // Defined in vi.h


        /* Paint the screen */
        refresh();


        /* Source the $HOME/.virc file */
        FILE *VIRC = fopen("$HOME/.virc", 'r');
        if(VIRC == NULL) error(".virc could not be opened");
        else {
                // Parse .virc file and do commands
                char *v = '';
                char *virc_line = "";
                while(v != EOF) {
                        do {
                                v = fgetc(VIRC);
                                virc_line = strncat(virc_line, v, 1);
                        } while(v != '\n');
                        if(strlen(virc_line) > 0) commandmode_main(virc_line);
                        virc_line = "";
                }

                // Clean up
                fclose(VIRC);
                free(virc_line);
                free(v);
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
                        else if(files[f] != NULL) {
                                char c = fgetc(files[f]);
                                while(c != EOF) {
                                        fputc(c, temp_files[f]);
                                        c = fgetc(files[f]);
                                }
                                rewind(temp_files[f]);
                                fclose(files[f]);
                                buffer_is_open[f] = true;
                        }
                        f++;
                }
        }


        /* Start visual mode (default) and go from there */
        for(unsigned char i=0; i<MAX_FILES; i++) work_saved[i] = true;
        /* Open temp file if no argument for filename was given */
        if(f == 0) {
                temp_file_names[f] = tempnam("/var/tmp/vi", NULL);
                temp_files[f] = fopen("/var/tmp/vi/"+temp_file_names[f], 'w');
                if(temp_files[f] == NULL) {error("Temp file could not be opened"); return 1;}
                else buffer_is_open[f] = true;
        }
        else f = 0;
        visualmode_main(f);


        /* Done with program, close temp files, free memory */
        for(unsigned char i=0; i<MAX_FILES; i++) {
                fclose("/var/tmp/vi/"+temp_files[i]);
                remove("/var/temp/vi/"+temp_files[i]);
        }
        delwin(editor_window);
        delwin(cmd_window);

        /* End program */
        endwin();
        return 0;
}

void
print(char *output)
{
        waddstr(cmd_window, output);
        refresh();
        sleep(1);
}

void
error(char *output)
{
        print(output);
}
