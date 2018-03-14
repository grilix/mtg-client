#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>
#include <string.h>

#include "bpwindow.h"
#include "bpmenu.h"

  static int
menu_loop(WINDOW *w, MENU *menu)
{
  int ch, loop = 1;

  keypad(w, TRUE);
  noecho();

  do {
    ch = wgetch(w);

    switch (ch)
    {
      case KEY_DOWN:
        menu_driver(menu, REQ_DOWN_ITEM);
      break;
      case KEY_UP:
        menu_driver(menu, REQ_UP_ITEM);
      break;
      case KEY_NPAGE:
        menu_driver(menu, REQ_SCR_DPAGE);
      break;
      case KEY_PPAGE:
        menu_driver(menu, REQ_SCR_UPAGE);
      break;
      case 10:
      case KEY_F(1):
        loop = false;
        break;
    }

    wrefresh(w);
  } while (loop == 1);

  return item_index(current_item(menu));
}

  extern int
bp_show_menu(char *title, char **options, int options_count,
             int sizex, int sizey, int x, int y)
{
  BpWindow *window;
  MENU *menu;
  ITEM **items, **item;
  int selected;

  items = (ITEM **)calloc(options_count + 1, sizeof(ITEM *));
  item = items;

  for (int i = 0; i < options_count; i++)
    *(item++) = new_item(options[i], NULL);

  menu = new_menu(items);

  window = bp_window_create_frame(title, sizex, sizey, x, y);

  set_menu_win(menu, window);
  set_menu_sub(menu, derwin(window, sizey - 4, sizex - 1, 3, 1));
  set_menu_format(menu, sizey - 4, 1);

  post_menu(menu);
  wrefresh(window);

  selected = menu_loop(window, menu);

  unpost_menu(menu);
  free_menu(menu);
  bp_window_destroy_clear(window);
  item = items;

  while (*item)
    free_item(*(item++));
  free(items);

  return selected;
}
