#include "vi.h"

gvoid redraw_screen();

gvoid
visualmode_main()
{

  redraw_screen();
  noecho();
  
  while(true) {

    wgetyx(editor_window[g], ypos[g], xpos[g]);
    gint visual_command = wgetch(editor_window[g]);

    switch(visual_command) {

      /* COMMAND MODE */
      case KEY_EIC:
        echo();
        commandmode_main("");
        redraw_screen();
        noecho();
        break;
      
      /* INSERT MODE */
      case 'i':
      case 'I':
      case 'a':
      case 'A':
      case 'o':
      case 'O':
      case 'R':
        echo();
        insertmode_main(visual_command, "");
        noecho();
        break;

      case 'r':
        echo();
        replace_one_ch();
        work_saved[g] = false;
        noecho();
        break;

      /* VISUAL MODE */
      case 'h':
      case KEY_LEFT:
        move_left();
        break;
      
      case 'j':
      case KEY_DOWN:
        move_down();
        break;
      
      case 'k':
      case KEY_UP:
        move_up();
        break;
      
      case 'l':
      case KEY_RIGHT:
        move_right();
        break;

      case 'w':
        move_to_next_word();
        break;

      case 'W':
        move_to_next_blank_delimited_word();
        break;

      case 'b':
        move_to_beginning_of_word();
        break;

      case 'B':
        move_to_beginning_blank_delimited_word();
        break;

      case '^':
        move_to_first_non_blank_ch_on_line();
        break;

      case '+':
      case KEY_ENTER:
        move_to_first_ch_next_line();
        break;

      case '-':
        move_to_first_non_blank_ch_previous_line();
        break;

      case 'e':
        move_to_end_of_word();
        break;

      case 'E':
        move_to_end_of_blank_delimited_word();
        break;

      case '(':
        move_a_sentence_back();
        break;

      case ')':
        move_a_sentence_forward();
        break;

      case '{':
        move_a_paragraph_back();
        break;

      case '}':
        move_a_paragaph_forward();
        break;

      case '%':
        move_to_associated_bracket();
        break;

      case '[':
        move_a_section_back();
        break;

      case ']':
        move_a_section_forward();
        break;

      case '0':
      case '|':
        move_to_beginning_of_line();
        break;

      case '$':
        move_to_end_of_line();
        break;

      case 'G':
        move_to_last_line();
        break;

      case ':':
        gchar *number = wgetstr(editor_window[g]);
        move_to_nth_line(number);
        break;

      case 'f':
        gint ch = wgetch(editor_window[g]);
        move_forward_to_ch(ch);
        break;

      case 'F':
        gint ch = wgetch(editor_window[g]);
        move_back_to_ch(ch);
        break;

      case 'H':
        move_to_top_of_screen();
        break;

      case 'M':
        move_to_middle_of_screen();
        break;

      case 'L':
        move_to_bottom_of_screen();
        break;

      case 'z':
        gint second_char = wgetch(editor_window[g]);
        switch(second_char) {
          case KEY_ENTER:
            make_current_line_top_line();
            break;

          case '.':
            make_current_line_middle_line();
            break;

          case '-':
            make_current_line_bottom_line();
            break;

          default:
            break;
        }

      case 36: /* Ctrl-d */
        move_forward_one_half_screen();
        break;

      case 38: /* Ctrl-f */
      case KEY_NPAGE:
        move_forward_one_full_screen();
        break;

      case 34: /* Ctrl-b */
      case KEY_PPAGE:
        move_back_one_full_screen();

      case 37: /* Ctrl-e */
        move_screen_up_one_line();
        break;

      case 57: /* Ctrl-y */
        move_screen_down_one_line();
        break;

      case 53: /* Ctrl-u */
        move_screen_up_one_half_page();
        break;

      case 44: /* Ctrl-l */
        redraw_screen();
        break;

      /* DELETE MODE */
      case 'x':
        delete_ch_under_cursor();
        work_saved[g] = false;
        break;

      case 'X':
        delete_ch_left_of_cursor();
        work_saved[g] = false;
        break;

      case 'D':
        delete_to_end_of_line();
        work_saved[g] = false;
        break;

      case 'd':
        gint second_char = wgetch(editor_window[g]);
        switch(second_char) {
          case '$':
            delete_from_cursor_to_end_of_line();
            work_saved[g] = false;
            break;

          case 'd':
            delete_current_line();
            work_saved[g] = false;
            break;

          case 'w':
            delete_next_word_starting_from_current();
            work_saved[g] = false;
            break;

          case 'b':
            delete_previous_word_starting_from_current();
            work_saved[g] = false;
            break;

          case 'G':
            delete_current_line_to_end_of_file();
            work_saved[g] = false;
            break;

          default:
            break;
        }

      /* MISCELLANEOUS */
      case '~':
        toggle_case_of_ch();
        work_saved[g] = false;
        break;

      case 'J':
        join_lines();
        work_saved[g] = false;
        break;

      case 'u':
        undo();
        work_saved[g] = false;
        break;

      /* YANK AND PASTE */
      case 'y':
        gint second_char = wgetch(editor_window[g]);
        switch(second_char) {
          case 'y':
            yank_current_line();
            break;

          case '$':
            yank_from_cursor_to_line_end();
            break;

          case 'w':
            yank_from_cursor_to_next_word();
            break;

          case 'G':
            yank_from_cursor_to_file_end();
            break;

          default:
            break;
        }

      case 'p':
        paste_after_position();
        break;

      case 'P':
        paste_before_position();
        break;

      default:
        break;
    }
  }
}
