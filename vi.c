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
                // Obtain file size
                fseek(VIRC , 0 , SEEK_END);
                unsigned long int file_size = ftell(VIRC);
                rewind(VIRC);

                // Allocate memory to contain the whole file
                char *virc_buffer = (char*)malloc(sizeof(char)*file_size);
                if(virc_buffer == NULL) {error("Memory error"); return 1;}

                // Copy the file into the buffer
                size_t result = fread(virc_buffer, 1, file_size, VIRC);
                if(result != file_size) {error("Reading error"); return 1;}

                // Parse .virc file and do commands
                char *virc_line = strtok(virc_buffer, '\n');
                while(virc_line != NULL) {
                        commandmode_main(virc_line);
                        virc_line = strtok(NULL, '\n');
                }

                // Clean up
                fclose(VIRC);
                free(virc_buffer);
                free(virc_line);
        }

        
        /* Set extern vars from cmdline opts using argc, argv*/
        f = 0;
        for(int i=1; i<argc; i++) {
                if(argv[i][0] == '-') {
                        /* Get command and execute */
                        char command[strlen(argv[i])];
                        command[0] = ':';
                        for(unsigned char j=1; j<strlen(argv[i]); j++) command[j] = argv[i][j];
                        char *hyphen = strchr(command, '-');
                        command[*hypen] = ' ';
                        commandmode_main(command);
                }
                else {
                        /* Open the file(s) */
                        if(f > TOTAL_FILE_NUM - 1) break;      /* Sanity check */
                        file_names[f] = argv[i];
                        files[f] = fopen(file_names[f], 'r');
                        if(files[f] == NULL) error("File "+file_names[f]+" could not be opened");
                        else {
                                fseek(files[f], 0, SEEK_SET);
                                file_poss[f] = ftell(file);
                                f++;
                        }
                }
        }


        /* Start visual mode (default) and go from there */
        for(unsigned char i=0; i<TOTAL_FILE_NUM; i++) work_saved[i] = true;
        f = 0;
        if(strlen(file_name[f]) > 0) visualmode_main(f);
        else visualmode_main();


        /* Done with program, free memory */
        delwin(editor_window);
        delwin(cmd_window);

        /* End program */
        endwin();
        return 0;
}
