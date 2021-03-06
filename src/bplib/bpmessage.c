#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include "bpwindow.h"
#include "bpmessage.h"

  extern void
bp_show_message(char *text, int x, int y)
{
  BpWindow *window = bp_window_create_box(strlen(text) + 4, 5, x, y);

  mvwprintw(window->_window, 2, 2, "%s", text);

  bp_window_getch(window);
  bp_window_destroy_clear(window);
}

  extern void
bp_show_plain_message(char *format, ...)
{
  refresh();
  def_prog_mode();
  endwin();

  va_list args;

  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  getch();

  reset_prog_mode();
  refresh();
}
