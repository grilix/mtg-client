#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>
#include "bplib/bpform.h"
#include "bplib/bpmenu.h"
#include "bplib/bpmessage.h"
#include "chest/session.h"
#include "chest/actions/actions.h"

static BpMenuItem
  menu_sign_in =    {.value = 0, .title = "Sign In"},
  menu_collection = {.value = 1, .title = "Collection"},
  menu_exit =       {.value = 2, .title = "Exit"},
  menu_sign_out =   {.value = 3, .title = "Sign Out"},
  menu_decks =      {.value = 4, .title = "Decks"};

  static void
main_menu(Session *session, BpMenu *menu)
{
  BpMenuItem *selected = NULL;
  int signed_in = session->token != NULL;

  int choices_count = 4;
  int current = 0;

  BpMenuItem **choices =
    (BpMenuItem **)calloc(choices_count, sizeof(BpMenuItem *));

  if (signed_in)
    choices[current++] = &menu_sign_out;
  else
    choices[current++] = &menu_sign_in;

  if (signed_in)
  {
    choices[current++] = &menu_collection;
    choices[current++] = &menu_decks;
  }
  else
    choices_count -= 2;

  choices[current++] = &menu_exit;

  bp_menu_set_items(menu, choices, choices_count);

  bp_menu_loop(menu);

  if (menu->_window->status == BP_WINDOW_STATUS_LOOPING)
  {
    selected = menu->selected;

    if (selected == &menu_exit)
    {
      menu->_window->status = BP_WINDOW_STATUS_COMPLETE;
    }
    else if (selected == &menu_collection)
    {
      collection_menu(session);
    }
    else if (selected == &menu_sign_in)
    {
      sign_in_form(session);
    }
    else if (selected == &menu_sign_out)
    {
      session_drop(session);
    }
    else if (selected == &menu_decks)
    {
      decks_menu(session);
    }
  }

  free(choices);
}

  static void
finish(int sig)
{
  endwin();
  exit(0);
}

  static void
main_loop(Session *session, int x, int y)
{
  BpMenu *menu = bp_menu_create("Options", NULL, 0, 20, 10, x, y);

  do {
    main_menu(session, menu);
  } while (menu->_window->status == BP_WINDOW_STATUS_LOOPING);

  bp_menu_destroy_clear(menu);
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

  main_loop(session, 15, 15);

  session_save(session, ".token");

  session_destroy(session);

  endwin();

  return 0;
}
