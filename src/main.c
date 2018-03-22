#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>

#include "bplib/bpform.h"
#include "bplib/bpmenu.h"
#include "bplib/bpmessage.h"
#include "bplib/bpwindow.h"
#include "chest/actions/actions.h"
#include "chest/session.h"

static BpMenuItem menu_sign_in = {.value = 0, .title = "Sign In"},
                  menu_collection = {.value = 1, .title = "Collection"},
                  menu_exit = {.value = 2, .title = "Exit"},
                  menu_sign_out = {.value = 3, .title = "Sign Out"},
                  menu_decks = {.value = 4, .title = "Decks"};

static void
main_menu_driver(Session *session, BpMenu *menu)
{
  BpMenuItem *selected = NULL;
  int x = menu->_window->x + 2, y = menu->_window->y + 2;

  bp_menu_loop(menu);

  if (menu->_window->status != BP_WINDOW_STATUS_LOOPING)
    return;

  selected = menu->selected;

  if (selected == &menu_exit)
  {
    menu->_window->status = BP_WINDOW_STATUS_COMPLETE;
    return;
  }

  if (selected == &menu_collection)
    collection_menu(session, x, y);
  else if (selected == &menu_sign_in)
    sign_in_form(session, x, y);
  else if (selected == &menu_sign_out)
    session_drop(session);
  else if (selected == &menu_decks)
    decks_menu(session, x, y);

  bp_window_refresh(menu->_window);
}

static int
main_menu_items(Session *session, BpMenuItem **items)
{
  int signed_in = session->token != NULL;
  int items_count = 4;
  int current = 0;

  if (signed_in)
    items[current++] = &menu_sign_out;
  else
    items[current++] = &menu_sign_in;

  if (signed_in)
  {
    items[current++] = &menu_collection;
    items[current++] = &menu_decks;
  }
  else
    items_count -= 2;

  items[current++] = &menu_exit;

  return items_count;
}

static void
main_menu(Session *session, int x, int y)
{
  int items_count = 4;

  BpMenu *menu = bp_menu_create("Options", NULL, 0, 20, 10, x, y);

  BpMenuItem **choices =
      (BpMenuItem **)calloc(items_count, sizeof(BpMenuItem *));

  do
  {
    items_count = main_menu_items(session, choices);

    bp_menu_set_items(menu, choices, items_count);

    main_menu_driver(session, menu);
  } while (menu->_window->status == BP_WINDOW_STATUS_LOOPING);

  free(choices);

  bp_menu_destroy_clear(menu);
}

static void
finish(int sig)
{
  endwin();
  exit(0);
}

extern int
main()
{
  signal(SIGINT, finish);

  initscr();
  refresh();

  start_color();

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);

  Session *session = session_allocate();

  session_load(session, ".token");

  main_menu(session, 15, 15);

  session_save(session, ".token");

  session_destroy(session);

  endwin();

  return 0;
}
