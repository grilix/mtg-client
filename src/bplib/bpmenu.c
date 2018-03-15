#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>
#include <string.h>

#include "bpwindow.h"
#include "bpmenu.h"

  extern void
bp_menu_loop(BpMenu *menu)
{
  int ch, loop = 1;

  keypad(menu->_window, TRUE);
  noecho();

  do {
    ch = wgetch(menu->_window);

    switch (ch)
    {
      case KEY_DOWN:
        menu_driver(menu->_menu, REQ_DOWN_ITEM);
      break;
      case KEY_UP:
        menu_driver(menu->_menu, REQ_UP_ITEM);
      break;
      case KEY_NPAGE:
        menu_driver(menu->_menu, REQ_SCR_DPAGE);
      break;
      case KEY_PPAGE:
        menu_driver(menu->_menu, REQ_SCR_UPAGE);
      break;
      case 10:
      case KEY_F(1):
        loop = false;
        break;
    }

    wrefresh(menu->_window);
  } while (loop == 1);

  menu->selected = menu->_menu_items[item_index(current_item(menu->_menu))];
}

  extern BpMenu *
bp_menu_create(char *title, BpMenuItem **options, int options_count,
              int sizex, int sizey, int x, int y)
{
  BpMenu *menu = (BpMenu *)malloc(sizeof(BpMenu));
  menu->_menu_items = options;

  ITEM **item;

  menu->_items = (ITEM **)calloc(options_count + 1, sizeof(ITEM *));
  item = menu->_items;

  for (int i = 0; i < options_count; i++)
    *(item++) = new_item(options[i]->title, NULL);

  menu->_menu = new_menu(menu->_items);

  menu->_window = bp_window_create_frame(title, sizex, sizey, x, y);

  set_menu_win(menu->_menu, menu->_window);
  set_menu_sub(menu->_menu,
               derwin(menu->_window, sizey - 4, sizex - 1, 3, 1));
  set_menu_format(menu->_menu, sizey - 4, 1);

  post_menu(menu->_menu);
  wrefresh(menu->_window);

  return menu;
}

  extern void
bp_menu_destroy_clear(BpMenu *menu)
{
  ITEM **item;

  unpost_menu(menu->_menu);
  free_menu(menu->_menu);
  bp_window_destroy_clear(menu->_window);

  item = menu->_items;
  while (*item)
    free_item(*(item++));
  free(menu->_items);
  free(menu);
}

  extern BpMenuItem *
bp_show_menu(char *title, BpMenuItem **options, int options_count,
             int sizex, int sizey, int x, int y)
{
  BpMenu *menu = bp_menu_create(title, options, options_count,
                                sizex, sizey, x, y);

  bp_menu_loop(menu);
  BpMenuItem *selected = menu->selected;

  bp_menu_destroy_clear(menu);
  return selected;
}
