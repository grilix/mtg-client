#include <menu.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "bpmenu.h"
#include "bpwindow.h"

extern void
bp_menu_driver(BpMenu *menu, int ch)
{
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
    menu->status = BP_WINDOW_STATUS_PROCESSING;
    break;
  }

  wrefresh(menu->_window->_window);

  if (menu->_menu_items != NULL)
    menu->selected = menu->_menu_items[item_index(current_item(menu->_menu))];
}

extern void
bp_menu_loop_start(BpMenu *menu)
{
  keypad(menu->_window->_window, TRUE);
  noecho();

  menu->status = menu->_window->status = BP_WINDOW_STATUS_LOOPING;
}

extern void
bp_menu_loop_handler(BpMenu *menu)
{
  int ch = bp_window_getch(menu->_window);

  if (!bp_window_driver(menu->_window, ch))
    bp_menu_driver(menu, ch);

  // If window is done, give up.
  if (menu->_window->status != BP_WINDOW_STATUS_LOOPING)
  {
    if (menu->status == BP_WINDOW_STATUS_LOOPING)
      menu->status = BP_WINDOW_STATUS_COMPLETE;
  }
}

extern void
bp_menu_loop(BpMenu *menu)
{
  bp_menu_loop_start(menu);

  do
  {
    bp_menu_loop_handler(menu);
  } while (menu->status == BP_WINDOW_STATUS_LOOPING);
}

static void
bp_menu_destroy_items(BpMenu *menu)
{
  if (menu->_items == NULL)
    return;

  free(menu->_items[0]);
  free(menu->_items);
}

extern void
bp_menu_item_destroy_list(BpMenuItem **items)
{
  if (items == NULL)
    return;

  free(items[0]);
  free(items);
}

extern BpMenuItem **
bp_menu_item_create_list(int count)
{
  BpMenuItem **items = (BpMenuItem **)calloc(count, sizeof(BpMenuItem *));
  if (items == NULL)
    return NULL;

  items[0] = (BpMenuItem *)calloc(count, sizeof(BpMenuItem));
  if (items[0] == NULL)
  {
    free(items);
    return NULL;
  }

  return items;
}

extern void
bp_menu_set_items(BpMenu *menu, BpMenuItem **items, int items_count)
{
  if (menu->_menu != NULL)
  {
    unpost_menu(menu->_menu);
    free_menu(menu->_menu);
  }

  bp_menu_destroy_items(menu);

  if (items_count == 0)
  {
    menu->_menu_items = NULL;
    menu->_items = NULL;
  }
  else
  {
    menu->_menu_items = items;

    menu->_items = (ITEM **)calloc(items_count + 1, sizeof(ITEM *));
    if (menu->_items == NULL)
      return;

    menu->_items[0] = (ITEM *)calloc(items_count + 1, sizeof(ITEM));
    if (menu->_items[0] == NULL)
    {
      free(menu->_items);
      menu->_items = NULL;
      return;
    }

    for (int i = 0; i < items_count; i++)
    {
      menu->_items[i] = menu->_items[0] + i;

      menu->_items[i]->name.str = items[i]->title;
      menu->_items[i]->name.length = (unsigned short)strlen(items[i]->title);

      menu->_items[i]->description.str = NULL;
      menu->_items[i]->description.length = 0;

      menu->_items[i]->opt = O_ONEVALUE;
    }
  }

  menu->_menu = new_menu(menu->_items);

  set_menu_win(menu->_menu, menu->_window->_window);
  set_menu_sub(
      menu->_menu,
      derwin(menu->_window->_window, menu->sizey - 4, menu->sizex - 1, 3, 1));
  set_menu_format(menu->_menu, menu->sizey - 4, 1);

  post_menu(menu->_menu);
  wrefresh(menu->_window->_window);
}

extern BpMenu *
bp_menu_create(char *title, BpMenuItem **options, int options_count, int sizex,
               int sizey, int x, int y)
{
  BpMenu *menu = (BpMenu *)malloc(sizeof(BpMenu));

  menu->_items = NULL;
  menu->_menu = NULL;
  menu->sizex = sizex;
  menu->sizey = sizey;
  menu->selected = NULL;

  menu->_window = bp_window_create_frame(title, sizex, sizey, x, y);

  bp_menu_set_items(menu, options, options_count);

  set_menu_win(menu->_menu, menu->_window->_window);
  set_menu_sub(menu->_menu,
               derwin(menu->_window->_window, sizey - 4, sizex - 1, 3, 1));
  set_menu_format(menu->_menu, sizey - 4, 1);

  post_menu(menu->_menu);
  wrefresh(menu->_window->_window);

  return menu;
}

extern void
bp_menu_destroy_clear(BpMenu *menu)
{
  unpost_menu(menu->_menu);
  free_menu(menu->_menu);
  bp_window_destroy_clear(menu->_window);

  bp_menu_destroy_items(menu);
  free(menu);
}
