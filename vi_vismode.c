#include "vi.h"

gvoid redraw_screen();

gvoid
visualmode_main(gint visual_command)
{
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

      /* COUNT and RANGE PREFIXES */
      case ':':
        visual_command = wgetch(editor_window[g]);
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        if (visual_command == '%') {
          count_0_is_first_and_count_1_is_last(count);
        }
        else if(visual_command == '.') {
          count_0_is_current_line(count[0]);
        }
        else if(visual_comand == '$') {
          count_0_is_last_line(count[0]);
        }
        else {
          count[0] = visual_command - 48; /* ASCII table manipulation */
          visual_command = wgetch(editor_window[g]);
        }
        if(visual_command == KEY_ENTER) {
          move_to_nth_line(count[0]);
          break;
        }
        else if(visual_command == ',') {
          visual_command = wgetch(editor_window[g]);
          if(visual_command == '.') count_1_is_current_line(count[1]);
          else if(visual_command == '$') count_1_is_last_line(count[1]);
          else count[1] = visual_command - 48; /* ASCII table manipulation */
        }
        visual_command = wgetch(editor_window[g]);
        /* No break */

      /* VISUAL MODE */
      case 'h':
      case KEY_LEFT:
        move_left(count[0]);
        break;
      
      case 'j':
      case KEY_DOWN:
        move_down(count[0]);
        break;
      
      case 'k':
      case KEY_UP:
        move_up(count[0]);
        break;
      
      case 'l':
      case KEY_RIGHT:
        move_right(count[0]);
        break;

      case 'w':
        move_to_next_word(count[0]);
        break;

      case 'W':
        move_to_next_blank_delimited_word(count[0]);
        break;

      case 'b':
        move_to_beginning_of_word(count[0]);
        break;

      case 'B':
        move_to_beginning_blank_delimited_word(count[0]);
        break;

      case '^':
        move_to_first_non_blank_ch_on_current_line(count[0]);
        break;

      case '+':
      case KEY_ENTER:
        move_to_first_ch_next_line(count[0]);
        break;

      case '-':
        move_to_first_non_blank_ch_previous_line(count[0]);
        break;

      case 'e':
        move_to_end_of_word(count[0]);
        break;

      case 'E':
        move_to_end_of_blank_delimited_word(count[0]);
        break;

      case '(':
        move_a_sentence_back(count[0]);
        break;

      case ')':
        move_a_sentence_forward(count[0]);
        break;

      case '{':
        move_a_paragraph_back(count[0]);
        break;

      case '}':
        move_a_paragaph_forward(count[0]);
        break;

      case '%':
        move_to_associated_bracket();
        break;

      case '[':
        move_a_section_back(count[0]);
        break;

      case ']':
        move_a_section_forward(count[0]);
        break;

      case '|':
        move_to_beginning_of_line(count[0]);
        break;

      case '$':
        move_to_end_of_line();
        break;

      case 'G':
        move_to_last_line(count[0]);
        break;

      case 'f':
        visual_command = wgetch(editor_window[g]);
        move_forward_to_ch(count[0], visual_command);
        break;

      case 'F':
        visual_command = wgetch(editor_window[g]);
        move_back_to_ch(count[0], visual_command);
        break;

      case 'H':
        move_to_top_of_screen(count[0]);
        break;

      case 'M':
        move_to_middle_of_screen(count[0]);
        break;

      case 'L':
        move_to_bottom_of_screen(count[0]);
        break;

      case 'z':
        visual_command = wgetch(editor_window[g]);
        switch(visual_command) {
          case KEY_ENTER:
            make_current_line_top_line(count[0]);
            break;

          case '.':
            make_current_line_middle_line(count[0]);
            break;

          case '-':
            make_current_line_bottom_line(count[0]);
            break;

          default:
            break;
        }

      case 36: /* Ctrl-d */
        move_forward_one_half_screen();
        break;

      case 38: /* Ctrl-f */
      case KEY_NPAGE: /* Page down */
        move_forward_one_full_screen();
        break;

      case 34: /* Ctrl-b */
      case KEY_PPAGE: /* Page up */
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
        undo(0); /* Parameter is an ascii_buffer_number (the default, zero) */
        work_saved[g] = false;
        break;

      /* BUFFERS */
      case '"':
        visual_command = wgetch(editor_window[g]);
        ascii_buffer_number = visual_command - 97; /* ASCII table manipulation */
        visual_command = wgetch(editor_window[g]);
        /* No break */

      /* DELETE MODE */
      case 'x':
        delete_ch_under_cursor(count[0], ascii_buffer_number);
        work_saved[g] = false;
        break;

      case 'X':
        delete_ch_left_of_cursor(count[0], ascii_buffer_number);
        work_saved[g] = false;
        break;

      case 'D':
        delete_to_end_of_line(ascii_buffer_number);
        work_saved[g] = false;
        break;

      case 'd':
        if(count[1] > 0) delete_range(count, ascii_buffer_number);
        else {
          visual_command = wgetch(editor_window[g]);
          switch(visual_command) {
            case '$':
              delete_from_cursor_to_end_of_line(ascii_buffer_number);
              work_saved[g] = false;
              break;

            case 'd':
              delete_current_line(count[0], ascii_buffer_number);
              work_saved[g] = false;
              break;

            case 'w':
              delete_next_word_starting_from_current(count[0], ascii_buffer_number);
              work_saved[g] = false;
              break;

            case 'b':
              delete_previous_word_starting_from_current(count[0], ascii_buffer_number);
              work_saved[g] = false;
              break;

            case 'G':
              delete_current_line_to_end_of_file(ascii_buffer_number);
              work_saved[g] = false;
              break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              gchar number[12];
              unsigned gchar i = 0;
              while(i<11 && visual_command != KEY_ENTER) {
                number[i++] = visual_command - 48; /* ASCII table manipulation */
                visual_command = wgetch(editor_window[g]);
              }
              number[i] = '\0';
              delete_until_end_of_sentence_num(atoi(number), ascii_buffer_number);
              break;

            default:
              break;
          }
        }

      /* YANK AND PASTE */
      case 'y':
        if(count[1] > 0) yank_range(count, ascii_buffer_number);
        else {
          visual_command = wgetch(editor_window[g]);
          switch(visual_command) {
            case 'y':
              yank_line_and_down(count[0], ascii_buffer_number);
              break;

            case '$':
              yank_from_cursor_to_line_end(ascii_buffer_number);
              break;

            case 'w':
              yank_from_cursor_to_next_word(count[0], ascii_buffer_number);
              break;

            case 'G':
              yank_from_cursor_to_file_end(ascii_buffer_number);
              break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              gchar number[12];
              unsigned gchar i = 0;
              while(i<11 && visual_command != KEY_ENTER) {
                number[i++] = visual_command - 48; /* ASCII table manipulation */
                visual_command = wgetch(editor_window[g]);
              }
              number[i] = '\0';
              yank_until_end_of_sentence_num(atoi(number), ascii_buffer_number);
              break;

            default:
              break;
          }
        }

      case 'p':
        paste_after_current_position(ascii_buffer_number);
        break;

      case 'P':
        paste_before_current_position(ascii_buffer_number);
        break;

      default:
        break;
    }
}
