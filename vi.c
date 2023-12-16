#include "vi.h"

int
main(int argc, char **argv)
{
        /* Initialize the program */
        /* TODO: Source the $HOME/.virc file */
        /* TODO: Set extern vars from cmdline opts using argc, argv*/
        /* TODO: Load file(s) to edit */

        /* Intitialize the screen */
        stdscr = initscr(); // Defined in vi.h
        raw();
        echo();
        keypad(stdscr, TRUE);

        getmaxyx(stdscr, maxy, maxx); // maxy, maxx defined in vi.h

        editor_window = newwin(0, 0, maxy-1, maxx); // Defined in vi.h
        cmd_window = newwin(maxy, 0, 1, maxx); // Defined in vi.h
        /* Done with Initialization */


        /* Paint the screen */
        refresh();


        /* Start visual mode (default) and go from there */
        work_saved = true;
        visualmode_main();


        /* Done with program, free memory */
        delwin(editor_window);
        delwin(cmd_window);

        /* End program */
        endwin();
        return 0;
}
