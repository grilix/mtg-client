#include <ncurses.h>
#include "bpwindow.h"

  extern BpWindow *
bp_window_create_frame(char *title, int sizex, int sizey, int x, int y)
{
  WINDOW *window = newwin(sizey, sizex, y, x);
  bp_window_draw_frame(title, window, sizex);

  return window;
}

  extern BpWindow *
bp_window_create_box(int sizex, int sizey, int x, int y)
{
  WINDOW *window = newwin(sizey, sizex, y, x);
  box(window, 0, 0);

  refresh();

  return window;
}

  extern void
bp_window_destroy(BpWindow *window)
{
  delwin(window);
}

  extern void
bp_window_destroy_clear(BpWindow *window)
{
  werase(window);
  wrefresh(window);
  bp_window_destroy(window);
}

  extern void
bp_window_draw_frame(char *title, BpWindow *window, int sizex)
{
  box(window, 0, 0);
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, 1, 2, "%s", title);
  wattroff(window, COLOR_PAIR(1));

  mvwaddch(window, 2, 0, ACS_LTEE);
  mvwhline(window, 2, 1, ACS_HLINE, sizex - 2);
  mvwaddch(window, 2, sizex - 1, ACS_RTEE);
}
