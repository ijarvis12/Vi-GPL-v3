#include "vi.h"

gvoid visualmode_main(gint visual_command) {

  unsigned gchar gtemp_undo = gbuffer[g].gundo;
  unsigned long gint i=0;
  unsigned long gint temp_range0=0;
  unsigned gchar c_char='\0';

  switch(visual_command) {

    /* COMMAND MODE */
    case KEY_EIC:
      range[0] = 0;
      range[1] = 0;
      echo();
      commandmode_main("");
      wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
      noecho();
      redraw_screen();
      break;
    
    /* INSERT MODE */
    case 'i':
    case 'I':
    case 'a':
    case 'A':
    case 'o':
    case 'O':
      range[0] = 0;
      range[1] = 0;
      insertmode_main(visual_command);
      break;

    case 'r':
      /* Replace a character */
      range[0] = 0;
      range[1] = 0;
      visual_command = wgetch(editor_windows[g]);
      visualmode_main('x');
      gchar vis_chs[2] = {(gchar)visual_command, '\0'};
      if(insert_chars(vis_chs)) next_gtemp();
      break;

    case 'R':
      /* Replace many characters */
      range[0] = 0;
      range[1] = 0;
      do {
        visual_command = wgetch(editor_windows[g]);
        switch(visual_command) {
          case KEY_LEFT:
            visualmode_main('h');
            break;

          case KEY_DOWN:
            visualmode_main('j');
            break;

          case KEY_UP:
            visualmode_main('k');
            break;

          case KEY_RIGHT:
            visualmode_main('l');
            break;

          case KEY_BACKSPACE:
            visualmode_main('X');
            break;

          default:
            visualmode_main('x');
            gchar vis_chs = {visual_command, '\0'};
            if(insert_chars(vis_chs)) next_gtemp();
            break;
        }
        wrefresh(editor_windows[g]);
      } while(visual_command != KEY_EIC);
      break;

    /* COUNT and RANGE PREFIXES */
    case ':':
      visual_command = wgetch(editor_windows[g]);
      if (visual_command == '%') {
        /* range[0] is first and range[1] is last */
        range[0] = 1;
        range[1] = gbuffer[g].gtotal_lines[gtemp_undo];
      }
      else if(visual_command == '.') {
        /* range[0] is current line */
        range[0] = gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo];
      }
      else if(visual_command == '$') {
        /* range[0] is last line */
        range[0] = gbuffer[g].gtotal_lines[gtemp_undo];
      }
      visual_command = wgetch(editor_windows[g]);
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
        i = 1;
        while(i<20 && visual_command < 58 && visual_command > 47) {
          number[i++] = visual_command;
          visual_command = wgetch(editor_windows[g]);
        }
        number[i] = '\0';
        range[0] = strtoul(number, NULL, 10);
      }
      if(visual_command == ',' && range[1] == 0) { /* Get range[1] */
        visual_command = wgetch(editor_windows[g]);
        if(visual_command == '.') range[1] = gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo]; /* range[1] is current line */
        else if(visual_command == '$') range[1] = gbuffer[g].gtotal_lines[gtemp_undo]; /* range[1] is last line */
        else { /* Else range[1] is a number */
          gchar number[21] = {visual_command};
          i = 1;
          while(i<20 && visual_command < 58 && visual_command > 47) {
            number[i++] = visual_command;
            visual_command = wgetch(editor_windows[g]);
          }
          number[i] = '\0';
          range[1] = strtoul(number, NULL, 10);
        }
        /* Sanity check on range[1] */
        if(range[0] >= range[1]) break; /* Go back to main loop and set range to {0, 0} */
      }
      /* Else if typed ':%' followed by a number, break and start over */
      else if(range[1] != 0) break;
      /* No break otherwise */

    /* VISUAL MODE */
    case 'h':
    case KEY_LEFT:
    case KEY_BACKSPACE:
      /* move left */
      i=0;
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
        else if(gbuffer[g].xpos[gtemp_undo] > 0) wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], --(gbuffer[g].xpos[gtemp_undo]));
        else {
          if(gbuffer[g].ypos[gtemp_undo] > 0) wmove(editor_windows[g], --(gbuffer[g].ypos[gtemp_undo]), maxx);
          else {
            visualmode_main(5); /* Scroll up */ 
            wmove(editor_windows[g], 0, maxx);
          }
          gbuffer[g].xpos[gtemp_undo] = maxx;
          do {
            c_char = winch(editor_windows[g]) & A_CHARTEXT;
            wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], --(gbuffer[g].xpos[gtemp_undo]));
          } while(c_char != 10); /* c_char != '\n' */
          getyx(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
        }
        fseek(gbuffer[g].gtemp_files[gtemp_undo], -1, SEEK_CUR);
        i++;
      } while(i < range[0]);
      break;
    
    case 'j':
    case KEY_DOWN:
      /* move down */
      i=0;
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
        else if(gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo] >= gbuffer[g].gtotal_lines[gtemp_undo]) {
          gchar *line = NULL;
          unsigned long gint len = 0;
          getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]);
          gbuffer[g].xpos[gtemp_undo] += strlen(line); /* update xpos */
          wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
          if(line != NULL) free(line);
          break;
        }
        else if(gbuffer[g].ypos[gtemp_undo] < maxy-1) {
          wmove(editor_windows[g], ++(gbuffer[g].ypos[gtemp_undo]), 0);
          gint j = gbuffer[g].xpos[gtemp_undo];
          while(fgetc(gbuffer[g].gtemp_files[gtemp_undo]) != 10 && j <= maxx) j++;
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          j=0;
          while(c_char != 10 || j < gbuffer[g].xpos[gtemp_undo]) {
            wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], ++j);
            c_char = winch(editor_windows[g]) & A_CHARTEXT;
          }
          getyx(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
          fseek(gbuffer[g].gtemp_files[gtemp_undo], gbuffer[g].xpos[gtemp_undo], SEEK_CUR);
        }
        else visualmode_main(25); /* Scroll down */
        i++;
      } while(i < range[0]);
      break;
    
    case 'k':
    case KEY_UP: /* ***TODO*** */
      /* move up */
      i=0;
      do {
        if(gbuffer[g].gtop_line[gtemp_undo] == 1) {
          wmove(editor_windows[g], 0, 0);
          fseek(gbuffer[g].gtemp_files[gtemp_undo] , 0, SEEK_SET);
          break;
        }
        else if(gbuffer[g].ypos[gtemp_undo] > 0) {
          wmove(editor_windows[g], --(gbuffer[g].ypos[gtemp_undo]), maxx);
          fseek(gbuffer[g].gtemp_files[gtemp_undo], -(gbuffer[g].xpos[gtemp_undo]), SEEK_CUR);
          gint j=maxx;
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          while(c_char != 10 && j > 0) { /* c_char != '\n' */
            wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], --j);
            c_char = winch(editor_windows[g]) & A_CHARTEXT;
          }
          if(j == 0) {
            fseek(gbuffer[g].gtemp_files[gtemp_undo], -1, SEEK_CUR);
          }
          else if(j >= gbuffer[g].xpos[gtemp_undo]) {
            wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
            fseek(gbuffer[g].gtemp_files[gtemp_undo], -(j-gbuffer[g].xpos[gtemp_undo]-1), SEEK_CUR);
          }
          else {/* j less than xpos */
            fseek(gbuffer[g].gtemp_files[gtemp_undo], -1, SEEK_CUR);
          }
          wgetyx(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
        }
        else visualmode_main(5); /* Scroll up */
        i++;
      } while(i < range[0]);
      break;
    
    case 'l':
    case KEY_RIGHT:
      /* move right */
      i=0;
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
        else if(gbuffer[g].xpos[gtemp_undo] < maxx) {
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          if(c_char != 10) {
            wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], ++(gbuffer[g].xpos[gtemp_undo]));
            fseek(gbuffer[g].gtemp_files[gtemp_undo], +1, SEEK_CUR);
          }
          else if(gbuffer[g].ypos[gtemp_undo] < maxy-1) {
            wmove(editor_windows[g], ++(gbuffer[g].ypos[gtemp_undo]), 0);
            gbuffer[g].xpos[gtemp_undo] = 0;
            fseek(gbuffer[g].gtemp_files[gtemp_undo], +1, SEEK_CUR);
          }
          else visualmode_main(25); /* Scroll down */
        }
        else if(gbuffer[g].ypos[gtemp_undo] < maxy-1) {
          wmove(editor_windows[g], ++(gbuffer[g].ypos[gtemp_undo]), 0);
          gbuffer[g].xpos[gtemp_undo] = 0;
          fseek(gbuffer[g].gtemp_files[gtemp_undo], +1, SEEK_CUR);
        }
        else visualmode_main(25); /* Scroll down */
        i++;
      } while(i < range[0]);
      break;

    case 'w':
      /* move to next word */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
        do { /* move through letters/numbers */
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while((c_char > 47 && c_char < 58) || (c_char > 64 && c_char < 91));
        do { /* move through everything else */
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91));
        i++;
      } while(i < temp_range0);
      break;

    case 'W':
      /* move to next blank delimited word */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
        do { /* move through non-blanks) */
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          /* while c_char != ' ', 't', '\n' */
        } while(c_char != 32 && c_char != 9 && c_char != 10);
        do { /* move through blanks */
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        i++;
      } while(i < temp_range0);
      break;

    case 'b':
      /* move to beginning of word */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
        do { /* move through non-letters/numbers */
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91));
        do { /* move through letters/numbers */
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while((c_char > 47 && c_char < 58) || (c_char > 64 && c_char < 91));
        i++;
      } while(i < temp_range0);
      /* move back once in the other direction if need be */
      if(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91) && ftell(gbuffer[g].gtemp_files[gtemp_undo]) != 0) {
        visualmode_main('l');
      }
      break;

    case 'B':
      /* move to beginning blank delimited word */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
        do { /* move through blanks */
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          /* while c_char == ' ', '\t', '\n' */
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        do { /* move through text (not blanks) */
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char != 32 && c_char != 9 && c_char != 10);
        i++;
      } while(i < temp_range0);
      /* move back once in the other direction if need be */
      if((c_char == 32 || c_char == 9 || c_char == 10) && ftell(gbuffer[g].gtemp_files[gtemp_undo]) != 0) {
        visualmode_main('l');
      }
      break;

    case '^':
      /* move to first non blank ch on current line */
      range[0] = 0;
      visualmode_main('|');
      c_char = winch(editor_windows[g]) & A_CHARTEXT;
      while(c_char == 32 || c_char == 9) { /* Note: '\n' not needed, but ' '  and '\t' are */
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
        visualmode_main('l');
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
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
    
      do { /* move back to end of previous word */
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
        visualmode_main('h');
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
      } while(!(c_char > 47 && c_char < 58) && !(c_char > 64 && c_char < 91));
      break;

    case 'E':
      /* move to end of blank delimited word */
      visualmode_main('W'); /* move to next word; range[0] still carries */
      range[0] = 0;
      do { /* move back to end of previous word */
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
        visualmode_main('h');
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
      } while(c_char == 32 || c_char == 9 || c_char == 10);
      break;

    case '(':
      /* move a sentence back */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'B' and 'E' moves */
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
        do {
          visualmode_main('B');
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('B');
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('E');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char != '.');
        i++;
      } while(i < temp_range0);
      visualmode_main('W'); /* One last move to put cursor on beginning of sentence */
      break;

    case ')':
      /* move a sentence forward */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'e' moves */
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
        do {
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('e');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char != '.');
        i++;
      } while(i < temp_range0);
      visualmode_main('W'); /* One last move to put cursor on beginning of sentence */
      break;

    case '{':
      /* move a paragraph back */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
        do { /* move through blanks */
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          /* while c_char == ' ', '\t', '\n' */
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        do { /* move through text */
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          /* while c_char != '\n' */
        } while(c_char != 10);
        i++;
      } while(i < temp_range0);
      if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) != 0) /* One last move to position at beginning of paragraph */
        visualmode_main('l');
      break;

    case '}':
      /* move a paragaph forward */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
        do { /* move through text */
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          /* while c_char != '\n' */
        } while(c_char != 10);
        do { /* move through blanks */
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
          /* while c_char == ' ', '\t', '\n' */
        } while(c_char == 32 || c_char == 9 || c_char == 10);
        i++;
      } while(i < temp_range0);
      break;

    case '%':
      /* move to associated bracket */
      c_char = winch(editor_windows[g]) & A_CHARTEXT;
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
      i=1;
      range[0] = 0; /* for move_char */
      range[1] = 0; /*      "        */
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
        else if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
        visualmode_main(move_char);
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
        if(c_char == char_orig) i++;
        else if(c_char == char_opp) i--;
      } while(i > 0);
      break;

    case '[':
      /* move a section back */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
        do {
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break; /* Sanity check */
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char != '{');
        i++;
      } while(i < temp_range0);
      break;

    case ']':
      /* move a section forward */
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
        do {
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break; /* Sanity check */
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char != '}');
        i++;
      } while(i < temp_range0);
      break;

    case '|':
      /* move to beginning of line, maybe offset column */
      c_char = winch(editor_windows[g]) & A_CHARTEXT;
      temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      while(c_char != 10) { /* while c_char != '\n' */
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
        visualmode_main('h');
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
        /* while c_char != '\n' */
      }
      if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) != 0 && !feof(gbuffer[g].gtemp_files[gtemp_undo])) visualmode_main('l');
      i=0;
      while(i < temp_range0 && c_char != 10) { /* c_char != '\n' */
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
        visualmode_main('l');
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
        i++;
      }
      break;

    case '$':
      /* move to end of line */
      range[0] = 0;
      range[1] = 0;
      c_char = winch(editor_windows[g]) & A_CHARTEXT;
      while(c_char != 10 && !feof(gbuffer[g].gtemp_files[gtemp_undo])) {
        visualmode_main('l');
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
      }
      break;

    case 'G':
      /* move to line default last */
      if(range[0] > 0 && range[0] < gbuffer[g].gtotal_lines[gtemp_undo]) gbuffer[g].gtop_line[gtemp_undo] = range[0];
      else gbuffer[g].gtop_line[gtemp_undo] = gbuffer[g].gtotal_lines[gtemp_undo];
      gbuffer[g].ypos[gtemp_undo] = 0;
      gbuffer[g].xpos[gtemp_undo] = 0;
      redraw_screen();
      break;

    case 'f':
      /* move forward to a specified character */
      visual_command = wgetch(editor_windows[g]);
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'l' moves */
      do {
        if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
        do {
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
          visualmode_main('l');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char != visual_command);
        i++;
      } while(i < temp_range0);
      break;

    case 'F':
      /* move back to a specified character */
      visual_command = wgetch(editor_windows[g]);
      i=0;
      temp_range0 = range[0];
      range[0] = 0; /* for 'h' moves */
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
        do {
          if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
          visualmode_main('h');
          c_char = winch(editor_windows[g]) & A_CHARTEXT;
        } while(c_char != visual_command);
        i++;
      } while(i < temp_range0);
      break;

    case 'H':
      /* move to top of screen */
      temp_range0 = range[0];
      range[0] = 0;
      range[1] = 0;
      while(gbuffer[g].ypos[gtemp_undo] > 0) visualmode_main('j');
      i=0;
      while(i < temp_range0 && gbuffer[g].ypos[gtemp_undo] < maxy - 1) {
        visualmode_main('k');
        i++;
      }
      break;

    case 'M':
      /* move to middle of screen */
      unsigned gint middle = maxy/2;
      range[0] = 0;
      range[1] = 0;
      if(gbuffer[g].ypos[gtemp_undo] < middle) {
        while(gbuffer[g].ypos[gtemp_undo] < middle) visualmode_main('k');
      }
      else {
        while(gbuffer[g].ypos[gtemp_undo] > middle) visualmode_main('j');
      }
      break;

    case 'L':
      /* move to bottom of screen */
      temp_range0 = range[0];
      range[0] = 0;
      range[1] = 0;
      while(gbuffer[g].ypos[gtemp_undo] < maxy - 1) visualmode_main('k');
      i=0;
      while(i < temp_range0 && gbuffer[g].ypos[gtemp_undo] > 0) {
        visualmode_main('j');
        i++;
      }
      break;

    case 'z':
      visual_command = wgetch(editor_windows[g]);
      switch(visual_command) {
        case 10: /* Carriage return */
          /* make current line, or range[0], top line */
          if(range[0] == 0) gbuffer[g].gtop_line[gtemp_undo] += gbuffer[g].ypos[gtemp_undo];
          else if(range[0] < gbuffer[g].gtotal_lines[gtemp_undo]) gbuffer[g].gtop_line[gtemp_undo] = range[0];
          else gbuffer[g].gtop_line[gtemp_undo] = gbuffer[g].gtotal_lines[gtemp_undo];
          gbuffer[g].ypos[gtemp_undo] = 0;
          gbuffer[g].xpos[gtemp_undo] = 0;
          redraw_screen();
          break;

        case '.':
          /* make current line, or range[0], middle line */
          unsigned long gint middle_line;
          gbuffer[g].ypos[gtemp_undo] = (maxy-1)/2;
          /* Set middle line */
          if(range[0] == 0 && (gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo]) < gbuffer[g].gtotal_lines[gtemp_undo]) {
            middle_line = gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo];
          }
          else if(range[0] < gbuffer[g].gtotal_lines[gtemp_undo]) middle_line = range[0];
          else {
            middle_line = gbuffer[g].gtotal_lines[gtemp_undo];
            gbuffer[g].ypos[gtemp_undo] = middle_line % (maxy-1);
          }
          /* Set top line */
          gbuffer[g].gtop_line[gtemp_undo] = middle_line - gbuffer[g].ypos[gtemp_undo] + 1;
          gbuffer[g].xpos[gtemp_undo] = 0;
          redraw_screen();
          break;

        case '-':
          /* make current line, or range[0], bottom line */
          unsigned long gint bottom_line;
          /* Set bottom line */
          if(range[0] == 0) bottom_line = gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo];
          else if(range[0] < gbuffer[g].gtotal_lines[gtemp_undo]) bottom_line = range[0];
          else bottom_line = gbuffer[g].gtotal_lines[gtemp_undo];
          /* Set top line */
          if(bottom_line > (unsigned long gint)(maxy - 1)) {
            gbuffer[g].gtop_line[gtemp_undo] = bottom_line - maxy + 1;
            gbuffer[g].ypos[gtemp_undo] = maxy - 1;
          }
          else {
            gbuffer[g].gtop_line[gtemp_undo] = 1;
            gbuffer[g].ypos[gtemp_undo] = bottom_line;
          }
          gbuffer[g].xpos[gtemp_undo] = 0;
          redraw_screen();
          break;

        default:
          break;
      }

    case 4: /* Ctrl-d */
      /* move forward one half screen */
      /* Set forward line */
      unsigned long gint forward = gbuffer[g].gtop_line[gtemp_undo] + (maxy-1)/2;
      /* Set top line */
      if(forward < gbuffer[g].gtotal_lines[gtemp_undo]) gbuffer[g].gtop_line[gtemp_undo] = forward;
      else {
        gbuffer[g].gtop_line[g] = gbuffer[g].gtotal_lines[gtemp_undo];
        gbuffer[g].ypos[gtemp_undo] = 0;
      }
      gbuffer[g].xpos[gtemp_undo] = 0;
      redraw_screen();
      break;

    case 6: /* Ctrl-f */
    case KEY_NPAGE: /* Page down */
      /* move forward one full screen */
      /* Set forward line */
      unsigned long gint forward = gbuffer[g].gtop_line[gtemp_undo] + maxy - 1;
      /* Set top line */
      if(forward < gbuffer[g].gtotal_lines[gtemp_undo]) gbuffer[g].gtop_line[gtemp_undo] = forward;
      else {
        gbuffer[g].gtop_line[gtemp_undo] = gbuffer[g].gtotal_lines[gtemp_undo];
        gbuffer[g].ypos[gtemp_undo] = 0;
      }
      gbuffer[g].xpos[gtemp_undo] = 0;
      redraw_screen();
      break;

    case 2: /* Ctrl-b */
    case KEY_PPAGE: /* Page up */
      /* move back one full_screen */
      unsigned long gint back = maxy-1;
      while(gbuffer[g].gtop_line[gtemp_undo] > 1) {
        gbuffer[g].gtop_line[gtemp_undo]--;
        back--;
        if(back == 0) break;
      }
      gbuffer[g].xpos[gtemp_undo] = 0;
      redraw_screen();
      break;

    case 5: /* Ctrl-e */
      /* move_screen_up_one_line */
      if(gbuffer[g].gtop_line[gtemp_undo] > 1) {
        gbuffer[g].gtop_line[gtemp_undo]--;
        gbuffer[g].xpos[gtemp_undo] = 0;
        redraw_screen();
      }
      break;

    case 25: /* Ctrl-y */
      /* move screen down one line */
      if(gbuffer[g].gtop_line[gtemp_undo] < gbuffer[g].gtotal_lines[gtemp_undo]) {
        gbuffer[g].gtop_line[gtemp_undo]++;
        if(gbuffer[g].ypos[gtemp_undo] > (gbuffer[g].gtotal_lines[gtemp_undo] - gbuffer[g].gtop_line[gtemp_undo])) {
          gbuffer[g].ypos[gtemp_undo] = gbuffer[g].gtotal_lines[gtemp_undo] - gbuffer[g].gtop_line[gtemp_undo];
        }
        gbuffer[g].xpos[gtemp_undo] = 0;
        redraw_screen();
      }
      break;

    case 21: /* Ctrl-u */
      /* move screen up one half page */
      unsigned long gint back2 = (maxy-1)/2;
      while(gbuffer[g].gtop_line[gtemp_undo] > 1) {
        gbuffer[g].gtop_line[gtemp_undo]--;
        back2--;
        if(back2 == 0) break;
      }
      gbuffer[g].xpos[gtemp_undo] = 0;
      redraw_screen();
      break;

    case 12: /* Ctrl-l */
      gbuffer[g].ypos[gtemp_undo] = 0;
      gbuffer[g].xpos[gtemp_undo] = 0;
      redraw_screen();
      break;

    case 7: /* Ctrl-g */
      /* Show filename */
      echo();
      gchar comm_chs[2] = {7, '\0'};
      commandmode_main(comm_chs);
      noecho();
      wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
      break;

    /* MISCELLANEOUS */
    case '~':
      /* toggle case of ch */
      c_char = winch(editor_windows[g]) & A_CHARTEXT;
      if(c_char > 64 && c_char < 91) c_char += 32;
      else if(c_char > 96 && c_char < 123) c_char -= 32;
      else break;
      waddch(editor_windows[g], c_char);
      wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
      fprintf(gbuffer[g].gtemp_files[gtemp_undo], "%c", c_char);
      fseek(gbuffer[g].gtemp_files[gtemp_undo], -1, SEEK_CUR);
      gbuffer[g].work_saved = false;
      break;

    case 'J':
      /* Join lines */
      unsigned gint temp_xpos = gbuffer[g].xpos[gtemp_undo];
      visualmode_main('$');
      visualmode_main('x'); /* work saved becomes false */
      gbuffer[g].xpos[gtemp_undo] = temp_xpos; /* restablish xpos */
      wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
      break;

    case 'u':
      /* undo */
      range[0] = 0;
      range[1] = 0;
      if(gtemp_undo > 0) {
        --gtemp_undo;
        --(gbuffer[g].gundo);
        redraw_screen();
      }
      break;

    case 'm':
      /* line markers */
      visual_command = wgetch(editor_windows[g]);
      if(visual_command > 96 && visual_command < 123) { 
        marker_line[visual_command-97] = gbuffer[g].gtop_line[gtemp_undo] + gbuffer[g].ypos[gtemp_undo];
      }
      break;

    case '`':
      /* go to line marker */
      visual_command = wgetch(editor_windows[g]);
      if(visual_command > 96 && visual_command < 123) {
        range[0] = marker_line[visual_command-97];
        visualmode_main('G');
      }
      break;

    /* BUFFERS */
    case '"':
      visual_command = wgetch(editor_windows[g]);
      if(visual_command > 96 && visual_command < 123) {
        gyank_num = visual_command - 97; /* ASCII table manipulation */
      }
      else break;
      visual_command = wgetch(editor_windows[g]);
      /* No break */

    /* DELETE MODE */
    case 'x':
      /* delete ch under cursor */
      if(!feof(gbuffer[g].gtemp_files[gtemp_undo])) {
        unsigned long gint gtemp_pos = ftell(gbuffer[g].gtemp_files[gtemp_undo]);
        i=0;
        do {
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
          gint del_char = fgetc(gbuffer[g].gtemp_files[gtemp_undo]);
          if(gyank_num > -1) fputc(del_char, gyank[gyank_num]);
          i++;
        } while(i < range[0]);
        gchar *line = NULL;
        unsigned long gint len = 0;
        GFILE *gtemporary_gfile = fopen("%%1", "w+");
        if(gtemporary_gfile == NULL ) {
          error("Undo not working, no temp file can be opened");
          fseek(gbuffer[g].gtemp_files[gtemp_undo], gtemp_pos, SEEK_SET);
          break;
        }
        while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0) {
          fprintf(gtemporary_gfile, "%s", line);
        }
        rewind(gtemporary_gfile);
        fseek(gbuffer[g].gtemp_files[gtemp_undo], gtemp_pos, SEEK_SET);
        while(getline(&line, &len, gtemporary_gfile) > 0) {
          fprintf(gbuffer[g].gtemp_files[gtemp_undo], "%s", line);
        }
        fputc(EOF, gbuffer[g].gtemp_files[gtemp_undo]);
        fseek(gbuffer[g].gtemp_files[gtemp_undo], gtemp_pos, SEEK_SET);
        gbuffer[g].work_saved = false;
        if(line != NULL) free(line);
        fclose("%%1");
        unlink("%%1");
        next_gtemp();
      }
      break;

    case 'X':
      /* delete ch left of cursor */
      temp_range0 = range[0];
      i=0;
      range[0] = 0;
      do {
        if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
        visualmode_main('h');
        visualmode_main('x');
        i++;
      } while(i < temp_range0);
      gbuffer[g].work_saved = false;
      break;

    case 'D':
      /* delete to end of line */
      range[0] = 0;
      c_char = winch(editor_windows[g]) & A_CHARTEXT;
      while(c_char != 10) { /* c_char != '\n' */
        visualmode_main('x');
        c_char = winch(editor_windows[g]) & A_CHARTEXT;
      }
      visualmode_main('x');
      gbuffer[g].work_saved = false;
      break;

    case 'd':
      if(range[1] > 0) { /* delete range */
        visualmode_main('G');
        for(unsigned long gint y=range[0]; y<=range[1]; y++) {
          if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
          visualmode_main('D');
        }
      }
      else {
        visual_command = wgetch(editor_windows[g]);
        switch(visual_command) {
        case '$':
          /* delete from cursor to end of line */
          visualmode_main('D');
          break;

        case 'd':
          /* delete current line */
          temp_range0 = range[0];
          i=0;
          do {
            visualmode_main('|');
            visualmode_main('D');
            i++;
          } while(i < temp_range0);
          break;

        case 'w':
          /* delete next word starting from current */
          temp_range0 = range[0];
          range[0] = 0;
          i=0;
          visualmode_main('b');
          unsigned gint gtemp_xprev;
          do {
            if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
            gtemp_xprev = gbuffer[g].xpos[gtemp_undo];
            visualmode_main('w');
            if(gbuffer[g].xpos[gtemp_undo] > gtemp_xprev) range[0] = gbuffer[g].xpos[gtemp_undo] - gtemp_xprev;
            else range[0] = maxx - gtemp_xprev + gbuffer[g].xpos[gtemp_undo] + 1;
            visualmode_main('X'); /* Capital 'X' */
            i++;
          } while(i < temp_range0);
          break;

        case 'b':
          /* delete previous word starting from current */
          unsigned long gint tmep_range0 = range[0];
          range[0] = 0;
          i=0;
          visualmode_main('w');
          unsigned gint gtemp_xprev2;
          do {
            if(ftell(gbuffer[g].gtemp_files[gtemp_undo]) == 0) break;
            gtemp_xprev2 = gbuffer[g].xpos[gtemp_undo];
            visualmode_main('b');
            if(gbuffer[g].xpos[gtemp_undo] < gtemp_xprev2) range[0] = gtemp_xprev2 - gbuffer[g].xpos[gtemp_undo];
            else range[0] = maxx - gbuffer[g].xpos[gtemp_undo] + gtemp_xprev2;
            visualmode_main('x'); /* Lower 'x' */
            i++;
          } while(i < temp_range0);
          break;

        case 'G':
          /* delete current line to end of file */
          while(!feof(gbuffer[g].gtemp_files[gtemp_undo])) visualmode_main('D');
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
          gchar number[21] = {visual_command};
          i = 1;
          while(i<20 && visual_command != 10 && visual_command < 58 && visual_command > 47) {
            number[i] = visual_command;
            visual_command = wgetch(editor_windows[g]);
            i++;
          }
          number[i] = '\0';
          /* delete until end of sentence num */
          unsigned long gint num = strtoul(number, NULL, 10);
          for(unsigned long gint y=(gbuffer[g].gtop_line[gtemp_undo]+gbuffer[g].ypos[gtemp_undo]); y<=num; y++) {
            if(feof(gbuffer[g].gtemp_files[gtemp_undo])) break;
            visualmode_main('D');
          }
          break;

        default:
          break;
        }
      }

    /* YANK AND PASTE */
    case 'y':
      /* Yank */
      if(gyank_num < 0) gyank_num = 0;
      visualmode_main('d');
      break;

    case 'p':
      /* paste after current position */
      unsigned long gint gtemp_pos = ftell(gbuffer[g].gtemp_files[gtemp_undo]);
      visualmode_main('P');
      fseek(gbuffer[g].gtemp_files[gbuffer[g].gundo], gtemp_pos, SEEK_SET);
      redraw_screen();
      break;

    case 'P':
      /* paste before current position */
      if(gyank_num < 0) gyank_num = 0;
      rewind(gyank[gyank_num]);
      gchar *line = NULL;
      unsigned long gint len = 0;
      gbool next = false;
      while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0) {
        next = insert_chars(line);
      }
      if(line != NULL) free(line);
      if(next) next_gtemp();
      fclose(gyank[gyank_num]);
      unlink(gyank_file_names[gyank_num]);
      gyank[gyank_num] = fopen(gyank_file_names[gyank_num], "w+");
      break;

    /* ELSE nothing */
    default:
      break;
    }
}

gvoid redraw_screen() {
  unsigned gchar gtemp_undo = gbuffer[g].gundo;
  /* File positioning for later */
  unsigned long gint gtemp_pos = ftell(gbuffer[g].gtemp_files[gtemp_undo]);
  rewind(gbuffer[g].gtemp_files[gtemp_undo]);
  /* Variables */
  gbuffer[g].gtotal_lines[gtemp_undo] = 1;
  unsigned long int i=1;
  gchar *line = NULL;
  unsigned long gint len = 0;
  do {
    if(i == gbuffer[g].gtop_line[gtemp_undo]) break;
    i++;
    gbuffer[g].gtotal_lines[gtemp_undo]++;
  } while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0);
  gint temp_ypos, temp_xpos, l, incr_l;
  /* For each line in the editor window */
  for(gint y=0; y<maxy; y++) {
    /* Get a line from the temp file*/
    if (getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0) {
      /* Add the line to the editor window */
      l = 0;
      incr_l = 0;
      while((l+incr_l) < strlen(line) && y<maxy) {
        do {
          /* add character */
          mvwaddch(editor_windows[g], y, l, line[l+incr_l]);
          l++;
        } while((l+incr_l) < strlen(line) && l <= maxx);
        incr_l += l + 1;
        l = 0;
        y++;
        gbuffer[g].gtotal_lines[gtemp_undo]++;
      }
      /* Get (temporary) xpos */
      getyx(editor_windows[g], temp_ypos, temp_xpos);
      /* Use xpos to set the rest of the line to blanks */
      for(gint x=temp_xpos; x<=maxx; x++) waddch(editor_windows[g], ' ');
    }
    /* Else fill line with blanks */
    else mvwhline(editor_windows[g], y, 0, ' ', maxx);
  }
  while(getline(&line, &len, gbuffer[g].gtemp_files[gtemp_undo]) > 0) gbuffer[g].gtotal_lines[gtemp_undo]++;
  if(line != NULL) free(line);
  /* Reset cursor in editor_windows */
  wmove(editor_windows[g], gbuffer[g].ypos[gtemp_undo], gbuffer[g].xpos[gtemp_undo]);
  fseek(gbuffer[g].gtemp_files[gtemp_undo], gtemp_pos, SEEK_SET);
  wrefresh(editor_windows[g]);
  return;
}
