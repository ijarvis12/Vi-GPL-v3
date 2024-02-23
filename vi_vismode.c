#include "vi.h"

gvoid redraw_screen();

gvoid visualmode_main(gint visual_command)
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
        /* range[0] is first and range[1] is last */
        range = {1, gtotal_lines[g]};
      }
      else if(visual_command == '.') {
        /* range[0] is current line */
        range[0] = gtop_line[g] + ypos[g];
      }
      else if(visual_comand == '$') {
        /* range[0] is last line */
        range[0] = gtotal_lines[g];
      }
      else { /* Else get range[0] */
        gchar number[21] = {visual_command};
        unsigned gchar i = 1;
        while(i<20 && visual_command < 58 && visual_command > 47) {
          number[i++] = visual_command;
          visual_command = wgetch(editor_window[g]);
        }
        number[i] = '\0';
        range[0] = strtoul(number, NULL, 10);
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
          gchar number[21] = {visual_command};
          unsigned gchar i = 1;
          while(i<20 && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          range[1] = strtoul(number, NULL, 10);
          free(number);
        }
      }
      /* No break */

    /* VISUAL MODE */
    case 'h':
    case KEY_LEFT: /* ***TODO*** */
      /* move left */
      unsigned long gint i=0;
      do {
        if(xpos[g] > 0) wmove(editor_window[g], ypos[g], xpos[g]--);
        else break;
        i++;
      } while(i < range[0]);
      break;
    
    case 'j':
    case KEY_DOWN: /* ***TODO*** */
      /* move down */
      unsigned long gint i=0;
      do {
        if(ypos[g] < maxy) wmove(editor_window[g], ypos[g]++, xpos[g]);
        else break;
        i++;
      } while(i < range[0]);
      break;
    
    case 'k':
    case KEY_UP: /* ***TODO*** */
      /* move up */
      unsigned long gint i=0;
      do {
        if(ypos[g] > 0) wmove(editor_window[g], ypos[g]--, xpos[g]);
        else break;
        i++;
      } while(i < range[0]);
      break;
    
    case 'l':
    case KEY_RIGHT: /* ***TODO*** */
      /* move right */
      unsigned long gint i=0;
      do {
        if(xpos[g] < maxx) wmove(editor_window[g], ypos[g], xpos[g]++);
        else break;
        i++;
      } while(i < range[0]);
      break;

    case 'w':
      /* move to next word */
      unsigned long gint i=0;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=1;
      range = {0, 0}; /* for move_char */
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
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g]) == 0) break;
        visualmode_main('h');
        char = winch(editor_window[g]);
        char = char | A_CHARTEXT;
      } while(char != '\n');
      if(ftell(temp_files[g]) != 0 && !feof(temp_files[g])) visualmode_main('l');
      unsigned long gint i=0;
      char = winch(editor_window[g]);
      char = char | A_CHAR_TEXT;
      while(i < temp_range0 && char != '\n') {
        if(feof(temp_files[g])) break;
        visualmode_main('l');
        char = winch(editor_window[g]);
        char = char | A_CHARTEXT;
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
        char = winch(editor_window[g]);
        char = char | A_CHARTEXT;
      }
      break;

    case 'G':
      /* move to line default last */
      if(range[0] > 0 && range[0] < gtotal_lines[g]) gtop_line[g] = range[0];
      else gtop_line[g] = gtotal_lines[g];
      ypos[g] = 0;
      xpos[g] = 0;
      redraw_screen(gtop_line[g]);
      break;

    case 'f':
      /* move forward to char */
      visual_command = wgetch(editor_window[g]);
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint i=0;
      gint char;
      unsigned long gint temp_range0 = range[0];
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
      unsigned long gint temp_range0 = range[0];
      range = {0, 0};
      while(ypos[g] > 0) visualmode_main('j');
      unsigned long gint i=0;
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
      unsigned long gint temp_range0 = range[0];
      range = {0, 0};
      while(ypos[g] < maxy - 1) visualmode_main('k');
      unsigned long gint i=0;
      while(i < temp_range0 && ypos[g] > 0) {
        visualmode_main('j');
        i++;
      }
      break;

    case 'z':
      visual_command = wgetch(editor_window[g]);
      switch(visual_command) {
        case KEY_ENTER:
          /* make current line, or range[0], top line */
          if(range[0] == 0) gtop_line[g] += ypos[g];
          else if(range[0] < gtotal_lines[g]) gtop_line[g] = range[0];
          else gtop_line[g] = gtotal_lines[g];
          ypos[g] = 0;
          xpos[g] = 0;
          redraw_screen(gtop_line[g]);
          break;

        case '.':
          /* make current line, or range[0], middle line */
          unsigned long gint middle_line;
          ypos[g] = (maxy-1)/2;
          /* Set middle line */
          if(range[0] == 0 && ypos[g] < gtotal_lines[g]) middle_line = gtop_line[g] + ypos[g];
          else if(range[0] < gtotal_lines[g]) middle_line = range[0];
          else {
            middle_line = gtotal_lines[g];
            ypos[g] = middle_line;
          }
          /* Set top line */
          gtop_line[g] = middle_line - ypos[g] + 1;
          xpos[g] = 0;
          redraw_screen(middle_line);
          break;

        case '-':
          /* make current line, or range[0], bottom line */
          unsigned long gint bottom_line;
          /* Set bottom line */
          if(range[0] == 0) bottom_line = gtop_line[g] + ypos[g];
          else if(range[0] < gtotal_lines[g]) bottom_line = range[0];
          else bottom_line = gtotal_lines[g];
          /* Set top line */
          if(bottom_line > (maxy - 1)) gtop_line[g] = bottom_line - maxy + 1;
          else gtop_line[g] = 1;
          ypos[g] = maxy-1;
          xpos[g] = 0;
          redraw_screen(bottom_line);
          break;

        default:
          break;
      }

    case 4: /* Ctrl-d */
      /* move forward one half screen */
      unsigned long gint forward;
      /* Set forward line */
      forward = gtop_line[g] + (maxy-1)/2;
      /* Set top line */
      if(forward < gtotal_lines[g]) gtop_line[g] = forward;
      else {
        gtop_lines[g] = gtotal_lines[g];
        ypos[g] = 0;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g]+ypos[g]);
      break;

    case 6: /* Ctrl-f */
    case KEY_NPAGE: /* Page down */
      /* move forward one full screen */
      unsigned long gint forward
      /* Set forward line */
      forward = gtop_line[g] + maxy - 1;
      /* Set top line */
      if(forward < gtotal_lines[g]) gtop_line[g] = forward;
      else {
        gtop_line[g] = gtotal_line[g];
        ypos[g] = 0;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g]+ypos[g]);
      break;

    case 2: /* Ctrl-b */
    case KEY_PPAGE: /* Page up */
      /* move back one full_screen */
      unsigned long gint back = maxy-1;
      while(gtop_line[g] > 1) {
        gtop_line[g]--;
        back--;
        if(back == 0) break;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g]+ypos[g]);
      break;

    case 5: /* Ctrl-e */
      /* move_screen_up_one_line */
      if(gtop_line[g] > 1) {
        gtop_line[g]--;
        xpos[g] = 0;
        redraw_screen(gtop_line[g]+ypos[g]);
      }
      break;

    case 25: /* Ctrl-y */
      /* move screen down one line */
      if(gtop_line[g] != gtotal_lines[g]) {
        gtop_line[g]++;
        if(ypos[g] > (gtotal_lines[g] - gtop_line[g])) ypos[g] = gtotal_lines[g] - gtop_line[g];
        xpos[g] = 0;
        redraw_screen(gtop_line[g]+ypos[g]);
      }
      break;

    case 21: /* Ctrl-u */
      /* move screen up one half page */
      unsigned long gint back = (maxy-1)/2;
      while(gtop_line[g] > 1) {
        gtop_line[g]--;
        back--;
        if(back == 0) break;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g]+ypos[g]);
      break;

    case 12: /* Ctrl-l */
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
          gchar number[21];
          unsigned gchar i = 0;
          while(i<20 && visual_command != KEY_ENTER && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          delete_until_end_of_sentence_num(strtoul(number, NULL, 10), ascii_buffer_number); /* ***TODO*** */
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
          gchar number[21];
          unsigned gchar i = 0;
          while(i<20 && visual_command != KEY_ENTER && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          yank_until_end_of_sentence_num(strtoul(number, NULL, 10), ascii_buffer_number); /* ***TODO*** */
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

gvoid redraw_screen(unsigned long gint gset_pos)
{
  rewind(temp_files[g]);
  unsigned long gint i=1;
  ghcar **line;
  while(i < gtop_line[g]) {
    getline(line, NULL, temp_files[g]);
    i++;
  }
  unsigned gint temp_ypos, temp_xpos;
  for(unsigned gint y=0; y<(maxy-1); y++) { /* For each line in the editor window */
    if(getline(line, NULL, temp_files[g]) > 0) { /* Get a line from the temp file*/
      /* Set the current pos variable if at the right line */
      if((gtop_line[g]+y) == gset_pos) gcurrent_pos[g] = ftell(temp_files[g]);
      mvwaddstr(editor_window[g], y, 0, *line); /* Add the line to the editor window */
      wgetxy(editor_window[g], temp_ypos, temp_xpos); /* Get xpos */
      /* Use xpos to set the rest of the line to blanks */
      for(unsigned gint x=temp_xpos; x<maxx; x++) waddch(editor_window[g], ' ');
    }
    else mvwhline(editor_window[g], y, 0, ' ', maxx);
  }
  free(line);
  wmove(editor_window[g], ypos[g], xpos[g]);
  fseek(temp_files[g], gcurrent_pos[g], SEEK_SET);
  wrefresh(editor_window[g]);
  return;
}
