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

        getyx(stdscr, maxy, maxx); // maxy, maxx defined in vi.h

        editor_window = newwin(0, 0, maxy-1, maxx);

        FIELD *editor_cmd_field = new_field(1, maxx, maxy, 0, 0, 0);
        field_opts_off(editor_cmd_field, 0_AUTOSKIP);
        editor_cmd_form = newform(editor_cmd_field); // Defined in vi.h
        post_form(editor_cmd_form);
        /* Done with Initialization */


        /* Paint the screen */
        refresh();


        /* Start visual mode (default) and go from there */
        visual_mode_main();


        /* Done with program, free memory */
        delwin(editor_screen);
        unpost_form(editor_cmd_form);
        free_form(editor_cmd_form);
        free_field(editor_cmd);

        /* End program */
        endwin();
        return 0;
}
