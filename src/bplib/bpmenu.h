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

  ITEM **_items;
  MENU *_menu;
  BpMenuItem **_menu_items;
  BpWindow *_window;
};

typedef struct _BpMenu BpMenu;

  extern BpMenuItem *
bp_show_menu(char *title, BpMenuItem **options, int options_count,
             int sizex, int sizey, int x, int y);

  extern void
bp_menu_loop(BpMenu *menu);

  extern BpMenu *
bp_menu_create(char *title, BpMenuItem **options, int options_count,
              int sizex, int sizey, int x, int y);

  extern void
bp_menu_destroy_clear(BpMenu *menu);

#endif
