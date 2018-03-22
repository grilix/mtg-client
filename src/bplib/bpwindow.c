#include <string.h>
#include <stdlib.h>

#include "bpwindow.h"

  extern void
bp_window_show_message(BpWindow *window, char *text)
{
  int sizex = strlen(text) + 8,
      sizey = 5,
      x = ((window->sizex - sizex) / 2) + window->x,
      y = ((window->sizey - sizey) / 2) + window->y;

  BpWindow *message_window = bp_window_create_box(sizex, sizey, x, y);

  mvwprintw(message_window->_window, 2, 4, "%s", text);

  bp_window_getch(message_window);
  bp_window_destroy_clear(message_window);
}

  extern int
bp_window_getch(BpWindow *window)
{
  return wgetch(window->_window);
}

  extern bool
bp_window_driver(BpWindow *window, int ch)
{
  switch (ch) {
    case 27:
      // Alt+x Closes the window
      ch = bp_window_getch(window);

      if (ch == 'x')
      {
        window->status = BP_WINDOW_STATUS_COMPLETE;
        return true;
      }
      break;
  }
  return false;
}

  extern void
bp_window_refresh(BpWindow *window)
{
  touchwin(window->_window);
}

  static BpWindow *
bp_window_create(int sizex, int sizey, int x, int y)
{
  BpWindow *window = (BpWindow *)malloc(sizeof(BpWindow));

  window->x = x;
  window->y = y;
  window->sizex = sizex;
  window->sizey = sizey;
  window->_window = newwin(sizey, sizex, y, x);

  return window;
}

  extern BpWindow *
bp_window_create_frame(char *title, int sizex, int sizey, int x, int y)
{
  BpWindow *window = bp_window_create(sizex, sizey, x, y);

  bp_window_draw_frame(title, window);

  return window;
}

  extern BpWindow *
bp_window_create_box(int sizex, int sizey, int x, int y)
{
  BpWindow *window = bp_window_create(sizex, sizey, x, y);

  box(window->_window, 0, 0);

  wrefresh(window->_window);

  return window;
}

  extern void
bp_window_destroy(BpWindow *window)
{
  delwin(window->_window);
  free(window);
}

  extern void
bp_window_destroy_clear(BpWindow *window)
{
  werase(window->_window);
  wrefresh(window->_window);
  bp_window_destroy(window);
}

  extern void
bp_window_draw_frame(char *title, BpWindow *window)
{
  box(window->_window, 0, 0);
  wattron(window->_window, COLOR_PAIR(1));
  mvwprintw(window->_window, 1, 2, "%s", title);
  wattroff(window->_window, COLOR_PAIR(1));

  mvwaddch(window->_window, 2, 0, ACS_LTEE);
  mvwhline(window->_window, 2, 1, ACS_HLINE, window->sizex - 2);
  mvwaddch(window->_window, 2, window->sizex - 1, ACS_RTEE);
}
