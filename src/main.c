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

  int
main_menu(Session *session)
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

  BpMenu *menu = bp_menu_create("Options", choices, choices_count,
                                20, 10, 15, 15);

  bp_menu_loop(menu);

  selected = menu->selected;

  free(choices);

  switch (selected->value)
  {
    case 0:
      sign_in_form(session);
    break;
    case 1:
      collection_menu(session);
    break;
    case 2:
      bp_menu_destroy_clear(menu);
      return -1;
    break;
    case 3:
      session_drop(session);
    break;
    case 4:
      decks_menu(session);
    break;
  }

  bp_menu_destroy_clear(menu);

  return selected->value;
}

  static void
finish(int sig)
{
  endwin();
  exit(0);
}

  int
main()
{
  signal(SIGINT, finish);

  int selected;

  initscr();
  refresh();

  start_color();

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
  Session *session = session_allocate();

  session_load(session, ".token");

  do {
    selected = main_menu(session);
  } while (selected != -1);

  session_save(session, ".token");

  session_destroy(session);

  endwin();

  return 0;
}
