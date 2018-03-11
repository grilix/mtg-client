#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>
#include <string.h>

#include "menu.h"

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

  static void
draw_menu_frame(char *title, WINDOW *w, int sizex)
{
  box(w, 0, 0);
  wattron(w, COLOR_PAIR(1));
  mvwprintw(w, 1, 1, "%s", title);
  wattroff(w, COLOR_PAIR(1));

  mvwaddch(w, 2, 0, ACS_LTEE);
  mvwhline(w, 2, 1, ACS_HLINE, sizex - 2);
  mvwaddch(w, 2, sizex - 1, ACS_RTEE);
}

  extern int
show_menu(char *title, char **options, int options_count,
          int sizex, int sizey, int x, int y)
{
  WINDOW *w;
  MENU *menu;
  ITEM **items, **item;
  int selected;

  items = (ITEM **)calloc(options_count + 1, sizeof(ITEM *));
  item = items;

  for (int i = 0; i < options_count; i++)
    *(item++) = new_item(options[i], NULL);

  menu = new_menu(items);

  w = newwin(sizey, sizex, y, x);

  draw_menu_frame(title, w, sizex);

  set_menu_win(menu, w);
  set_menu_sub(menu, derwin(w, sizey - 4, sizex - 1, 3, 1));
  set_menu_format(menu, sizey - 4, 1);

  post_menu(menu);
  wrefresh(w);

  selected = menu_loop(w, menu);

  unpost_menu(menu);
  free_menu(menu);
  delwin(w);
  item = items;

  while (*item)
    free_item(*(item++));
  free(items);

  return selected;
}
