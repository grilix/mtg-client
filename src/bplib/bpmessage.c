#include <string.h>
#include <ncurses.h>

#include "bpwindow.h"
#include "bpmessage.h"


  extern void
bp_show_message(char *text, int x, int y)
{
  BpWindow *window = bp_window_create_box(strlen(text) + 4, 5, x, y);

  mvwprintw(window, 2, 2, "%s", text);

  wrefresh(window);

  getch();
  bp_window_destroy_clear(window);
}
