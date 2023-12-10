#include "vi.h"
#include "vi_cmdmode.h"
#include "vi_insmode.h"
#include "vi_vismode.h"

int
main(int argc, char **argv)
{
        /* Initialize the program */
        /* TODO: Source the .virc file */
        /* TODO: Set extern vars from cmdline opts using argc, argv*/

        /* Intitialize the screen */
        stdscr = initscr(); // Defined in vi.h
        raw();
        echo();
        keypad(stdscr, TRUE);

        getmaxyx(stdscr, maxy, maxx); // maxy, maxx defined in vi.h

        editor_window = newwin(0, 0, maxy-1, maxx);
        cmd_window = newwin(maxy, 0, 1, maxx);
        /* Done with Initialization */


        /* Paint the screen */
        refresh();


        /* Start visual mode (default) and go from there */
        visual_mode_main();


        /* Done with program, free memory */
        delwin(editor_window);
        delwin(cmd_window);

        /* End program */
        endwin();
        return 0;
}
