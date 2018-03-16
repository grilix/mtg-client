#ifndef BPLIB_BPMENU_H
#define BPLIB_BPMENU_H

#include <menu.h>

#include "bpwindow.h"

struct _BpMenuItem
{
  char *title;
  int value;
};

typedef struct _BpMenuItem BpMenuItem;

struct _BpMenu
{
  BpMenuItem *selected;
  int sizex, sizey;
  BpWindowStatus status;

  ITEM **_items;
  MENU *_menu;
  BpMenuItem **_menu_items;
  BpWindow *_window;
};

typedef struct _BpMenu BpMenu;

  extern void
bp_menu_loop_start(BpMenu *menu);

  extern void
bp_menu_loop_handler(BpMenu *menu);

  extern void
bp_menu_loop(BpMenu *menu);

  extern BpMenu *
bp_menu_create(char *title, BpMenuItem **items, int items_count,
              int sizex, int sizey, int x, int y);

  extern void
bp_menu_set_items(BpMenu *menu, BpMenuItem **items, int items_count);

  extern void
bp_menu_destroy_clear(BpMenu *menu);

#endif
