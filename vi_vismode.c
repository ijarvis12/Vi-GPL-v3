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
      replace_one_ch(); /* ***TODO*** */
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
        /* range0 is first and range1 is last */
        range = {1, gtotal_lines[g]};
      }
      else if(visual_command == '.') {
        /* range0 is current line */
        range[0] = gtop_line[g] + ypos[g];
      }
      else if(visual_comand == '$') {
        /* range0 is last line */
        range[0] = gtotal_lines[g];
      }
      else { /* Else get range[0] */
        gchar number[32] = {visual_command};
        unsigned gchar i = 1;
        while(i<31 && visual_command < 58 && visual_command > 47) {
          number[i++] = visual_command;
          visual_command = wgetch(editor_window[g]);
        }
        number[i] = '\0';
        range[0] = atoi(number);
        free(number);
      }
      if(visual_command == KEY_ENTER) {
        visualmode_main('G'); /* range[0] still carries */
        break;
      }
      else if(visual_command == ',') { /* Else get range[1] */
        visual_command = wgetch(editor_window[g]);
        if(visual_command == '.') range[1] = gtop_line[g] + ypos[g]; /* range[1] is current line */
        else if(visual_command == '$') range[1] = gtotal_lines[g]; /* range[1] is last line */
        else { /* Else range[1] is a number */
          gchar number[32] = {visual_command};
          unsigned gchar i = 1;
          while(i<31 && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          range[1] = atoi(number);
          free(number);
        }
      }
      /* No break */

    /* VISUAL MODE */
    case 'h':
    case KEY_LEFT: /* ***TODO*** */
      /* move left */
      unsigned gint i=0;
      do {
        if(xpos[g] > 0) wmove(editor_window[g], ypos[g], xpos[g]--);
        else break;
        i++;
      } while(i < range[0]);
      break;
    
    case 'j':
    case KEY_DOWN: /* ***TODO*** */
      /* move down */
      unsigned gint i=0;
      do {
        if(ypos[g] < maxy) wmove(editor_window[g], ypos[g]++, xpos[g]);
        else break;
        i++;
      } while(i < range[0]);
      break;
    
    case 'k':
    case KEY_UP: /* ***TODO*** */
      /* move up */
      unsigned gint i=0;
      do {
        if(ypos[g] > 0) wmove(editor_window[g], ypos[g]--, xpos[g]);
        else break;
        i++;
      } while(i < range[0]);
      break;
    
    case 'l':
    case KEY_RIGHT: /* ***TODO*** */
      /* move right */
      unsigned gint i=0;
      do {
        if(xpos[g] < maxx) wmove(editor_window[g], ypos[g], xpos[g]++);
        else break;
        i++;
      } while(i < range[0]);
      break;

    case 'w':
      /* move to next word */
      unsigned gint i=0;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      gint char;
      do {
        if(feof(temp_files[g])) break; /* Sanity check */
        do { /* move through letters/numbers */
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while((char > 47 && char < 58) || (char > 64 && char < 91));
        do { /* move through everything else */
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(!(char > 47 && char < 58) && !(char > 64 && char < 91));
        i++;
      } while(i < temp_range0);
      break;

    case 'W':
      /* move to next blank delimited word */
      unsigned gint i=0;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      gint char;
      do {
        if(feof(temp_files[g])) break; /* Sanity check */
        do { /* move through non-blanks) */
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != ' ' && char != '\t' && char != '\n');
        do { /* move through blanks */
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char == ' ' || char == '\t' || char == '\n');
        i++;
      } while(i < temp_range0);
      break;

    case 'b':
      /* move to beginning of word */
      unsigned gint i=0;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      gint char;
      do {
        if(ftell(temp_files[g]) == 0) break; /* Sanity check */
        do { /* move through non-letters/numbers */
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(!(char > 47 && char < 58) && !(char > 64 && char < 91));
        do { /* move through letters/numbers */
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while((char > 47 && char < 58) || (char > 64 && char < 91));
        i++;
      } while(i < temp_range0);
      /* move back once in the other direction if need be */
      if(!(char > 47 && char < 58) && !(char > 64 && char < 91) && ftell(temp_files[g]) != 0) {
        visualmode_main('l');
      }
      break;

    case 'B':
      /* move to beginning blank delimited word */
      unsigned gint i=0;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      gint char;
      do {
        if(ftell(temp_files[g]) == 0) break; /* Sanity check */
        do { /* move through blanks */
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char == ' ' || char == '\t' || char == '\n');
        do { /* move through text (not blanks) */
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != ' ' && char != '\t' && char != '\n');
        i++;
      } while(i < temp_range0);
      /* move back once in the other direction if need be */
      if((char == ' ' || char == '\t' || char == '\n') && ftell(temp_files[g]) != 0) {
        range[0] = 0;
        visualmode_main('l');
      }
      break;

    case '^':
      /* move to first non blank ch on current line */
      range[0] = 0;
      gint char;
      visualmode_main('|');
      char = winch(editor_window[g]);
      char = char | A_CHARTEXT;
      while(char == ' ' || char == '\t') { /* Note: '\n' not needed */
        visualmode_main('l');
        char = winch(editor_window[g]);
        char = char | A_CHARTEXT;
      }
      break;

    case '+':
    case KEY_ENTER:
      /* move to first ch next line */
      visualmode_main('j'); /* range[0] works here */
      visualmode_main('^'); /* range[0] unset here */
      break;

    case '-':
      /* move to first non blank ch previous line */
      visualmode_main('k'); /* range[0] works here */
      visualmode_main('^'); /* range[0] unset here */
      break;

    case 'e':
      /* move to end of word */
      visualmode_main('w'); /* move to next word; range[0] still carries */
      range[0] = 0;
      gint char;
      do { /* move back to end of previous word */
        if(ftell(temp_files[g]) == 0) break; /* Sanity check */
        visualmode_main('h');
        char = winch(editor_window[g]);
        char = char | A_CHARTEXT;
      } while(!(char > 47 && char < 58) && !(char > 64 && char < 91));
      break;

    case 'E':
      /* move to end of blank delimited word */
      visualmode_main('W'); /* move to next word; range[0] still carries */
      range[0] = 0;
      gint char;
      do { /* move back to end of previous word */
        if(ftell(temp_files[g]) == 0) break; /* Sanity check */
        visualmode_main('h');
        char = winch(editor_window[g]);
        char = char | A_CHARTEXT;
      } while(char == ' ' || char == '\t' || char == '\n');
      break;

    case '(':
      /* move a sentence back */
      unsigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'B' and 'E' moves */
      do {
        if(ftell(temp_files[g]) == 0) break; /* Sanity check */
        do {
          visualmode_main('B');
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('B');
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('E');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != '.');
        i++;
      } while(i < temp_range0);
      visualmode_main('W'); /* One last move to put cursor on beginning of sentence */
      break;

    case ')':
      /* move a sentence forward */
      unsigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'e' moves */
      do {
        if(feof(temp_files[g])) break; /* Sanity check */
        do {
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('e');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != '.');
        i++;
      } while(i < temp_range0);
      visualmode_main('W'); /* One last move to put cursor on beginning of sentence */
      break;

    case '{':
      /* move a paragraph back */
      usigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g]) == 0) break; /* Sanity check */
        do { /* move through blanks */
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char == ' ' || char == '\t' || char == '\n');
        do { /* move through text */
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != '\n');
        i++;
      } while(i < temp_range0);
      if(ftell(temp_files[g]) != 0) /* One last move to position at beginning of paragraph */
        visualmode_main('l');
      break;

    case '}':
      /* move a paragaph forward */
      usigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(temp_files[g])) break; /* Sanity check */
        do { /* move through text */
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != '\n');
        do { /* move through blanks */
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char == ' ' || char == '\t' || char == '\n');
        i++;
      } while(i < temp_range0);
      break;

    case '%':
      /* move to associated bracket */
      gint char = winch(editor_window[g]);
      char = char | A_CHARTEXT;
      gint char_orig = char;
      gint char_opp;
      char move_char;
      if     (char_orig == '{') {char_opp = '}'; move_char = 'l';}
      else if(char_orig == '(') {char_opp = ')'; move_char = 'l';}
      else if(char_orig == '[') {char_opp = ']'; move_char = 'l';}
      else if(char_orig == '}') {char_opp = '{'; move_char = 'h';}
      else if(char_orig == ')') {char_opp = '('; move_char = 'h';}
      else if(char_orig == ']') {char_opp = '['; move_char = 'h';}
      else break;
      unsigned gint i=1;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for move_char */
      do {
        if(feof(temp_files[g])) break;
        else if(ftell(temp_files[g]) == 0) break;
        visualmode_main(move_char);
        char = winch(editor_window[g]);
        char = char | A_CHARTEXT;
        if(char == char_orig) i++;
        else if(char == char_opp) i--;
      } while(i > 0);
      break;

    case '[':
      /* move a section back */
      unsigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g]) == 0) break; /* Sanity check */
        do {
          if(ftell(temp_files[g]) == 0) break; /* Sanity check */
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != '{');
        i++;
      } while(i < temp_range0);
      break;

    case ']':
      /* move a section forward */
      unsigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(temp_files[g])) break; /* Sanity check */
        do {
          if(feof(temp_files[g])) break; /* Sanity check */
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != '}');
        i++;
      } while(i < temp_range0);
      break;

    case '|':
      /* move to beginning of line, maybe offset column */
      gint char = winch(editor_window[g]);
      char = char | A_CHARTEXT;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      while(char != '\n') {
        if(ftell(temp_files[g]) == 0) break;
        visualmode_main('h');
      }
      if(ftell(temp_files[g]) != 0) visualmode_main('l');
      unsigned gint i=0;
      while(i < temp_range0) {
        if(feof(temp_files[g])) break;
        visualmode_main('l');
      }
      break;

    case '$':
      /* move to end of line */
      range = {0, 0};
      gint char = winch(editor_window[g]);
      char = char | A_CHARTEXT;
      while(char != '\n') {
        if(feof(temp_files[g])) break;
        visualmode_main('l');
      }
      break;

    case 'G':
      /* move to line default last */
      if(range[0] > 0 && range[0] < total_lines[g]) gtop_line[g] = range[0];
      else gtop_line[g] = total_lines[g];
      ypos[g] = 0;
      xpos[g] = 0;
      rewind(temp_files[g]);
      unsigned long gint i=0;
      gchar **line;
      do {
        gcurrent_pos[g] = ftell(temp_files[g]);
        i++;
        if(i == gtop_line[g]) break;
      } while(getline(line, NULL, temp_files[g]) > 0);
      free(line);
      if(feof(temp_files[g])) visualmode_main('|');
      redraw_screen();
      break;

    case 'f':
      /* move forward to char */
      visual_command = wgetch(editor_window[g]);
      unsigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(temp_files[g])) break;
        do {
          if(feof(temp_files[g])) break;
          visualmode_main('l');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != visual_command)
        i++;
      } while(i < temp_range0);
      break;

    case 'F':
      /* move back to char */
      visual_command = wgetch(editor_window[g]);
      unsigned gint i=0;
      gint char;
      unsigned gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g]) == 0) break;
        do {
          if(ftell(temp_files[g]) == 0) break;
          visualmode_main('h');
          char = winch(editor_window[g]);
          char = char | A_CHARTEXT;
        } while(char != visual_command)
        i++;
      } while(i < temp_range0);
      break;

    case 'H':
      /* move to top of screen */
      unsigned gint temp_range0 = range[0];
      range = {0, 0};
      while(ypos[g] > 0) visualmode_main('j');
      unsigned gint i=0;
      while(i < temp_range0 && ypos[g] < maxy - 1) {
        visualmode_main('k');
        i++;
      }
      break;

    case 'M':
      /* move to middle of screen */
      unsigned gint middle = maxy/2;
      range = {0, 0};
      if(ypos[g] < middle) {
        while(ypos[g] < middle) visualmode_main('k');
      }
      else {
        while(ypos[g] > middle) visualmode_main('j');
      }
      break;

    case 'L':
      /* move to bottom of screen */
      unsigned gint temp_range0 = range[0];
      range = {0, 0};
      while(ypos[g] < maxy - 1) visualmode_main('k');
      unsigned gint i=0;
      while(i < temp_range0 && ypos[g] > 0) {
        visualmode_main('j');
        i++;
      }
      break;

    case 'z':
      visual_command = wgetch(editor_window[g]);
      switch(visual_command) {
        case KEY_ENTER:
        make_current_line_top_line(range[0]); /* ***TODO*** */
        break;

        case '.':
        make_current_line_middle_line(range[0]); /* ***TODO*** */
        break;

        case '-':
        make_current_line_bottom_line(range[0]); /* ***TODO*** */
        break;

        default:
        break;
      }

    case 36: /* Ctrl-d */
      move_forward_one_half_screen(); /* ***TODO*** */
      break;

    case 38: /* Ctrl-f */
    case KEY_NPAGE: /* Page down */
      move_forward_one_full_screen(); /* ***TODO*** */
      break;

    case 34: /* Ctrl-b */
    case KEY_PPAGE: /* Page up */
      move_back_one_full_screen(); /* ***TODO*** */

    case 37: /* Ctrl-e */
      move_screen_up_one_line(); /* ***TODO*** */
      break;

    case 57: /* Ctrl-y */
      move_screen_down_one_line(); /* ***TODO*** */
      break;

    case 53: /* Ctrl-u */
      move_screen_up_one_half_page(); /* ***TODO*** */
      break;

    case 44: /* Ctrl-l */
      redraw_screen(); /* ***TODO*** */
      break;

    /* MISCELLANEOUS */
    case '~':
      toggle_case_of_ch(); /* ***TODO*** */
      work_saved[g] = false;
      break;

    case 'J':
      join_lines(); /* ***TODO*** */
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
      delete_ch_under_cursor(range[0], ascii_buffer_number); /* ***TODO*** */
      work_saved[g] = false;
      break;

    case 'X':
      delete_ch_left_of_cursor(range[0], ascii_buffer_number); /* ***TODO*** */
      work_saved[g] = false;
      break;

    case 'D':
      delete_to_end_of_line(ascii_buffer_number); /* ***TODO*** */
      work_saved[g] = false;
      break;

    case 'd':
      if(range[1] > 0) delete_range(range, ascii_buffer_number); /* ***TODO*** */
      else {
        visual_command = wgetch(editor_window[g]);
        switch(visual_command) {
        case '$':
          delete_from_cursor_to_end_of_line(ascii_buffer_number); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'd':
          delete_current_line(range[0], ascii_buffer_number); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'w':
          delete_next_word_starting_from_current(range[0], ascii_buffer_number); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'b':
          delete_previous_word_starting_from_current(range[0], ascii_buffer_number); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'G':
          delete_current_line_to_end_of_file(ascii_buffer_number); /* ***TODO*** */
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
          gchar number[32];
          unsigned gchar i = 0;
          while(i<31 && visual_command != KEY_ENTER && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          delete_until_end_of_sentence_num(atoi(number), ascii_buffer_number); /* ***TODO*** */
          break;

        default:
          break;
        }
      }

    /* YANK AND PASTE */
    case 'y':
      if(range[1] > 0) yank_range(range, ascii_buffer_number);
      else {
        visual_command = wgetch(editor_window[g]);
        switch(visual_command) {
        case 'y':
          yank_line_and_down(range[0], ascii_buffer_number); /* ***TODO*** */
          break;

        case '$':
          yank_from_cursor_to_line_end(ascii_buffer_number); /* ***TODO*** */
          break;

        case 'w':
          yank_from_cursor_to_next_word(range[0], ascii_buffer_number); /* ***TODO*** */
          break;

        case 'G':
          yank_from_cursor_to_file_end(ascii_buffer_number); /* ***TODO*** */
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
          gchar number[32];
          unsigned gchar i = 0;
          while(i<31 && visual_command != KEY_ENTER && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          yank_until_end_of_sentence_num(atoi(number), ascii_buffer_number); /* ***TODO*** */
          break;

        default:
          break;
        }
      }

    case 'p':
      paste_after_current_position(ascii_buffer_number); /* ***TODO*** */
      break;

    case 'P':
      paste_before_current_position(ascii_buffer_number); /* ***TODO*** */
      break;

    default:
      break;
    }
}
