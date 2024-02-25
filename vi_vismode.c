#include "vi.h"

gvoid redraw_screen();

gvoid visualmode_main(gint visual_command)
{
  switch(visual_command) {

    /* COMMAND MODE */
    case KEY_EIC:
      range = {0, 0};
      echo();
      commandmode_main("");
      xpos[g] = 0;
      wmove(editor_window[g], ypos[g], xpos[g]);
      redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]);
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
      range = {0, 0};
      echo();
      insertmode_main(visual_command, "");
      noecho();
      break;

    case 'r':
      range = {0, 0};
      echo();
      replace_one_ch(); /* ***TODO*** */
      work_saved[g] = false;
      noecho();
      break;

    /* COUNT and RANGE PREFIXES */
    case ':':
      visual_command = wgetch(editor_window[g]);
      if (visual_command == '%') {
        /* range[0] is first and range[1] is last */
        range = {1, gtotal_lines[g][gtemp[g]]};
      }
      else if(visual_command == '.') {
        /* range[0] is current line */
        range[0] = gtop_line[g][gtemp[g]] + ypos[g];
      }
      else if(visual_comand == '$') {
        /* range[0] is last line */
        range[0] = gtotal_lines[g][gtemp[g]];
      }
      visual_command = wgetch(editor_window[g]);
      /* no break */
    
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
      if(range[0] == 0) { /* Get range[0] */
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
      if(visual_command == ',' && range[1] == 0) { /* Get range[1] */
        visual_command = wgetch(editor_window[g]);
        if(visual_command == '.') range[1] = gtop_line[g][gtemp[g]] + ypos[g]; /* range[1] is current line */
        else if(visual_command == '$') range[1] = gtotal_lines[g][gtemp[g]]; /* range[1] is last line */
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
      gint c_char;
      do {
        if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
        do { /* move through letters/numbers */
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_window[g]) & ACHARTEXT;
        } while((c_char > 47 && c_char < 58) || (c_char > 64 && c_char < 91));
        do { /* move through everything else */
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_window[g]) & ACHARTEXT;
        } while(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91));
        i++;
      } while(i < temp_range0);
      break;

    case 'W':
      /* move to next blank delimited word */
      unsigned long gint i=0;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      gint c_char;
      do {
        if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
        do { /* move through non-blanks) */
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
          /* while c_char != ' ', 't', '\n' */
        } while(c_char != 32 && c_char != 9 && c_char != 10);
        do { /* move through blanks */
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        i++;
      } while(i < temp_range0);
      break;

    case 'b':
      /* move to beginning of word */
      unsigned long gint i=0;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      gint c_char;
      do {
        if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
        do { /* move through non-letters/numbers */
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91));
        do { /* move through letters/numbers */
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while((c_char > 47 && c_char < 58) || (c_char > 64 && c_char < 91));
        i++;
      } while(i < temp_range0);
      /* move back once in the other direction if need be */
      if(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91) && ftell(temp_files[g][gtemp[g]]) != 0) {
        visualmode_main('l');
      }
      break;

    case 'B':
      /* move to beginning blank delimited word */
      unsigned long gint i=0;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      gint c_char;
      do {
        if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
        do { /* move through blanks */
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
          /* while c_char == ' ', '\t', '\n' */
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        do { /* move through text (not blanks) */
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char != 32 && c_char != 9 && c_char != 10);
        i++;
      } while(i < temp_range0);
      /* move back once in the other direction if need be */
      if((c_char == 32 || c_char == 9 || c_char == 10) && ftell(temp_files[g][gtemp[g]]) != 0) {
        range[0] = 0;
        visualmode_main('l');
      }
      break;

    case '^':
      /* move to first non blank ch on current line */
      range[0] = 0;
      gint c_char;
      visualmode_main('|');
      c_char = winch(editor_window[g]) & A_CHARTEXT;
      while(c_char == 32 || c_char == 9) { /* Note: '\n' not needed, but ' '  and '\t' are */
        visualmode_main('l');
        c_char = winch(editor_window[g]) & A_CHARTEXT;
      }
      break;

    case '+':
    case 10: /* Carriage return */
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
      gint c_char;
      do { /* move back to end of previous word */
        if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
        visualmode_main('h');
        c_char = winch(editor_window[g]) & A_CHARTEXT;
      } while(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91));
      break;

    case 'E':
      /* move to end of blank delimited word */
      visualmode_main('W'); /* move to next word; range[0] still carries */
      range[0] = 0;
      gint c_char;
      do { /* move back to end of previous word */
        if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
        visualmode_main('h');
        c_char = winch(editor_window[g]) & A_CHARTEXT;
      } while(c_char == 32 || c_char == 9 || c_char == 10);
      break;

    case '(':
      /* move a sentence back */
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'B' and 'E' moves */
      do {
        if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
        do {
          visualmode_main('B');
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('B');
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('E');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char != '.');
        i++;
      } while(i < temp_range0);
      visualmode_main('W'); /* One last move to put cursor on beginning of sentence */
      break;

    case ')':
      /* move a sentence forward */
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'e' moves */
      do {
        if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
        do {
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('e');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char != '.');
        i++;
      } while(i < temp_range0);
      visualmode_main('W'); /* One last move to put cursor on beginning of sentence */
      break;

    case '{':
      /* move a paragraph back */
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
        do { /* move through blanks */
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
          /* while c_char == ' ', '\t', '\n' */
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        do { /* move through text */
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
          /* while c_char != '\n' */
        } while(c_char != 10);
        i++;
      } while(i < temp_range0);
      if(ftell(temp_files[g][gtemp[g]]) != 0) /* One last move to position at beginning of paragraph */
        visualmode_main('l');
      break;

    case '}':
      /* move a paragaph forward */
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
        do { /* move through text */
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
          /* while c_char != '\n' */
        } while(c_char != 10);
        do { /* move through blanks */
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
          /* while c_char == ' ', '\t', '\n' */
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        i++;
      } while(i < temp_range0);
      break;

    case '%':
      /* move to associated bracket */
      gint c_char = winch(editor_window[g]) & A_CHARTEXT;
      gint char_orig = c_char;
      gint char_opp;
      gint move_char;
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
        if(feof(temp_files[g][gtemp[g]])) break;
        else if(ftell(temp_files[g][gtemp[g]]) == 0) break;
        visualmode_main(move_char);
        c_char = winch(editor_window[g]) & A_CHARTEXT;
        if(c_char == char_orig) i++;
        else if(c_char == char_opp) i--;
      } while(i > 0);
      break;

    case '[':
      /* move a section back */
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
        do {
          if(ftell(temp_files[g][gtemp[g]]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char != '{');
        i++;
      } while(i < temp_range0);
      break;

    case ']':
      /* move a section forward */
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
        do {
          if(feof(temp_files[g][gtemp[g]])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char != '}');
        i++;
      } while(i < temp_range0);
      break;

    case '|':
      /* move to beginning of line, maybe offset column */
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g][gtemp[g]]) == 0) break;
        visualmode_main('h');
        c_char = winch(editor_window[g]) & A_CHARTEXT;
        /* while c_char != '\n' */
      } while(c_char != 10);
      if(ftell(temp_files[g][gtemp[g]]) != 0 && !feof(temp_files[g][gtemp[g]])) visualmode_main('l');
      unsigned long gint i=0;
      do {
        if(feof(temp_files[g][gtemp[g]])) break;
        visualmode_main('l');
        c_char = winch(editor_window[g]) & A_CHARTEXT;
        i++;
        /* c_char != '\n' */
      } while(i < temp_range0 && c_char != 10);
      break;

    case '$':
      /* move to end of line */
      range = {0, 0};
      gint c_char = winch(editor_window[g]) & A_CHARTEXT;
      while(c_char != 10 && !feof(temp_files[g][gtemp[g]])) {
        visualmode_main('j');
        c_char = winch(editor_window[g]) & A_CHARTEXT;
      }
      break;

    case 'G':
      /* move to line default last */
      if(range[0] > 0 && range[0] < gtotal_lines[g][gtemp[g]]) gtop_line[g][gtemp[g]] = range[0];
      else gtop_line[g][gtemp[g]] = gtotal_lines[g][gtemp[g]];
      ypos[g] = 0;
      xpos[g] = 0;
      redraw_screen(gtop_line[g][gtemp[g]]);
      break;

    case 'f':
      /* move forward to a specified character */
      visual_command = wgetch(editor_window[g]);
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(temp_files[g][gtemp[g]])) break;
        do {
          if(feof(temp_files[g][gtemp[g]])) break;
          visualmode_main('l');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char != visual_command)
        i++;
      } while(i < temp_range0);
      break;

    case 'F':
      /* move back to a specified character */
      visual_command = wgetch(editor_window[g]);
      unsigned long gint i=0;
      gint c_char;
      unsigned long gint temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(temp_files[g][gtemp[g]]) == 0) break;
        do {
          if(ftell(temp_files[g][gtemp[g]]) == 0) break;
          visualmode_main('h');
          c_char = winch(editor_window[g]) & A_CHARTEXT;
        } while(c_char != visual_command)
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
        case 10: /* Carriage return */
          /* make current line, or range[0], top line */
          if(range[0] == 0) gtop_line[g][gtemp[g]] += ypos[g];
          else if(range[0] < gtotal_lines[g][gtemp[g]]) gtop_line[g][gtemp[g]] = range[0];
          else gtop_line[g][gtemp[g]] = gtotal_lines[g][gtemp[g]];
          ypos[g] = 0;
          xpos[g] = 0;
          redraw_screen(gtop_line[g][gtemp[g]]);
          break;

        case '.':
          /* make current line, or range[0], middle line */
          unsigned long gint middle_line;
          ypos[g] = (maxy-1)/2;
          /* Set middle line */
          if(range[0] == 0 && ypos[g] < gtotal_lines[g][gtemp[g]]) middle_line = gtop_line[g][gtemp[g]] + ypos[g];
          else if(range[0] < gtotal_lines[g][gtemp[g]]) middle_line = range[0];
          else {
            middle_line = gtotal_lines[g][gtemp[g]];
            ypos[g] = middle_line;
          }
          /* Set top line */
          gtop_line[g][gtemp[g]] = middle_line - ypos[g] + 1;
          xpos[g] = 0;
          redraw_screen(middle_line);
          break;

        case '-':
          /* make current line, or range[0], bottom line */
          unsigned long gint bottom_line;
          /* Set bottom line */
          if(range[0] == 0) bottom_line = gtop_line[g][gtemp[g]] + ypos[g];
          else if(range[0] < gtotal_lines[g][gtemp[g]]) bottom_line = range[0];
          else bottom_line = gtotal_lines[g][gtemp[g]];
          /* Set top line */
          if(bottom_line > (maxy - 1)) gtop_line[g][gtemp[g]] = bottom_line - maxy + 1;
          else gtop_line[g][gtemp[g]] = 1;
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
      forward = gtop_line[g][gtemp[g]] + (maxy-1)/2;
      /* Set top line */
      if(forward < gtotal_lines[g][gtemp[g]]) gtop_line[g][gtemp[g]] = forward;
      else {
        gtop_lines[g] = gtotal_lines[g][gtemp[g]];
        ypos[g] = 0;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]);
      break;

    case 6: /* Ctrl-f */
    case KEY_NPAGE: /* Page down */
      /* move forward one full screen */
      unsigned long gint forward
      /* Set forward line */
      forward = gtop_line[g][gtemp[g]] + maxy - 1;
      /* Set top line */
      if(forward < gtotal_lines[g][gtemp[g]]) gtop_line[g][gtemp[g]] = forward;
      else {
        gtop_line[g][gtemp[g]] = gtotal_lines[g][gtemp[g]];
        ypos[g] = 0;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]);
      break;

    case 2: /* Ctrl-b */
    case KEY_PPAGE: /* Page up */
      /* move back one full_screen */
      unsigned long gint back = maxy-1;
      while(gtop_line[g][gtemp[g]] > 1) {
        gtop_line[g][gtemp[g]]--;
        back--;
        if(back == 0) break;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]);
      break;

    case 5: /* Ctrl-e */
      /* move_screen_up_one_line */
      if(gtop_line[g][gtemp[g]] > 1) {
        gtop_line[g][gtemp[g]]--;
        xpos[g] = 0;
        redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]);
      }
      break;

    case 25: /* Ctrl-y */
      /* move screen down one line */
      if(gtop_line[g][gtemp[g]] < gtotal_lines[g][gtemp[g]]) {
        gtop_line[g][gtemp[g]]++;
        if(ypos[g] > (gtotal_lines[g][gtemp[g]] - gtop_line[g][gtemp[g]])) ypos[g] = gtotal_lines[g][gtemp[g]] - gtop_line[g][gtemp[g]];
        xpos[g] = 0;
        redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]);
      }
      break;

    case 21: /* Ctrl-u */
      /* move screen up one half page */
      unsigned long gint back = (maxy-1)/2;
      while(gtop_line[g][gtemp[g]] > 1) {
        gtop_line[g][gtemp[g]]--;
        back--;
        if(back == 0) break;
      }
      xpos[g] = 0;
      redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]);
      break;

    case 12: /* Ctrl-l */
      ypos[g] = 0;
      xpos[g] = 0;
      redraw_screen(gtop_line[g][gtemp[g]]);
      break;

    case 7: /* Ctrl-g */
      /* Show filename */
      echo();
      commandmode_main(7);
      noecho();
      wmove(editor_window[g], ypos[g], xpos[g]);
      break;

    /* MISCELLANEOUS */
    case '~':
      /* toggle case of ch */
      gint c_char = winch(editor_window[g]) & A_CHARTEXT;
      if(c_char > 64 && c_char < 91) c_char += 32;
      else if(c_char > 96 && c_char < 123) c_char -= 32;
      else break;
      waddch(editor_window[g], c_char);
      wmove(editor_window[g], ypos[g], xpos[g]);
      fprintf(temp_files[g][gtemp[g]], "%s", c_char);
      fseek(temp_files[g][gtemp[g]], gcurrent_pos[g], SEEK_SET);
      work_saved[g] = false;
      break;

    case 'J':
      /* Join lines */
      if(gtotal_lines[g][gtemp[g]] == 1 || (gtop_line[g][gtemp[g]]+ypos[g]) >= gtotal_lines[g][gtemp[g]]) break; /* Sanity check */
      unsigned gint temp_xpos = xpos[g];
      visualmode_main('$');
      visualmode_main('x'); /* work saved becomes false */
      xpos[g] = temp_xpos;
      wmove(editor_window[g], ypos[g], xpos[g]); /* restablish xpos */
      break;

    case 'u':
      /* undo */
      range = {0, 0};
      if(gtemp[g] > 0) {
        /* ***TODO*** */
        redraw_screen(gtop_line[g][gtemp[g]]+ypos[g]); /* ypos[g] could be non-zero */
      }
      break;

    /* BUFFERS */
    case '"':
      visual_command = wgetch(editor_window[g]);
      if(visual_command > 96 && visual_command < 123) {
      gyank_num = visual_command - 97; /* ASCII table manipulation */
      }
      else break;
      visual_command = wgetch(editor_window[g]);
      /* No break */

    /* DELETE MODE */
    case 'x':
      delete_ch_under_cursor(range[0], gyank_num, gundo_num[g]); /* ***TODO*** */
      
      work_saved[g] = false;
      break;

    case 'X':
      delete_ch_left_of_cursor(range[0], gyank_num, gundo_num[g]); /* ***TODO*** */
      work_saved[g] = false;
      break;

    case 'D':
      delete_to_end_of_line(gyank_num, gundo_num[g]); /* ***TODO*** */
      work_saved[g] = false;
      break;

    case 'd':
      if(range[1] > 0) delete_range(range, gyank_num, gundo_num[g]); /* ***TODO*** */
      else {
        visual_command = wgetch(editor_window[g]);
        switch(visual_command) {
        case '$':
          delete_from_cursor_to_end_of_line(gyank_num, gundo_num[g]); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'd':
          delete_current_line(range[0], gyank_num, gundo_num[g]); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'w':
          delete_next_word_starting_from_current(range[0], gyank_num, gundo_num[g]); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'b':
          delete_previous_word_starting_from_current(range[0], gyank_num, gundo_num[g]); /* ***TODO*** */
          work_saved[g] = false;
          break;

        case 'G':
          delete_current_line_to_end_of_file(gyank_num, gundo_num[g]); /* ***TODO*** */
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
          while(i<20 && visual_command != 10 && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          delete_until_end_of_sentence_num(strtoul(number, NULL, 10), gyank_num, gundo_num[g]); /* ***TODO*** */
          break;

        default:
          break;
        }
      }

    /* YANK AND PASTE */
    case 'y':
      if(range[1] > 0) yank_range(range, gyank_num, gundo_num[g]);
      else {
        visual_command = wgetch(editor_window[g]);
        switch(visual_command) {
        case 'y':
          yank_line_and_down(range[0], gyank_num, gundo_num[g]); /* ***TODO*** */
          break;

        case '$':
          yank_from_cursor_to_line_end(gyank_num, gundo_num[g]); /* ***TODO*** */
          break;

        case 'w':
          yank_from_cursor_to_next_word(range[0], gyank_num, gundo_num[g]); /* ***TODO*** */
          break;

        case 'G':
          yank_from_cursor_to_file_end(gyank_num, gundo_num[g]); /* ***TODO*** */
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
          while(i<20 && visual_command != 10 && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_window[g]);
          }
          number[i] = '\0';
          yank_until_end_of_sentence_num(strtoul(number, NULL, 10), gyank_num, gundo_num[g]); /* ***TODO*** */
          break;

        default:
          break;
        }
      }

    case 'p':
      paste_after_current_position(gyank_num, gundo_num[g]); /* ***TODO*** */
      break;

    case 'P':
      paste_before_current_position(gyank_num, gundo_num[g]); /* ***TODO*** */
      break;

    default:
      break;
    }
}

gvoid redraw_screen(unsigned long gint gset_pos)
{
  rewind(temp_files[g][gtemp[g]]);
  unsigned long gint i=1;
  gchar **line;
  while(i < gtop_line[g][gtemp[g]]) {
    getline(line, NULL, temp_files[g][gtemp[g]]);
    i++;
  }
  unsigned gint temp_ypos, temp_xpos;
  gcurrent_pos[g] = 0;
  for(unsigned gint y=0; y<(maxy-1); y++) { /* For each line in the editor window */
    /* Set the current pos variable if at the right line */
    if((gtop_line[g][gtemp[g]]+y) == gset_pos) gcurrent_pos[g] = ftell(temp_files[g][gtemp[g]]);
    if {(getline(line, NULL, temp_files[g][gtemp[g]]) > 0) { /* Get a line from the temp file*/
      mvwaddnstr(editor_window[g], y, 0, *line, maxx); /* Add the line to the editor window */
      wgetxy(editor_window[g], temp_ypos, temp_xpos); /* Get xpos */
      /* Use xpos to set the rest of the line to blanks */
      for(unsigned gint x=temp_xpos; x<maxx; x++) waddch(editor_window[g], ' ');
    }
    else mvwhline(editor_window[g], y, 0, ' ', maxx);
  }
  if(gcurrent_pos[g] == 0) ypos[g] = 0; /* Sanity check in case gset_pos never encountered */
  free(line);
  wmove(editor_window[g], ypos[g], xpos[g]);
  fseek(temp_files[g][gtemp[g]], gcurrent_pos[g], SEEK_SET);
  wrefresh(editor_window[g]);
  return;
}
