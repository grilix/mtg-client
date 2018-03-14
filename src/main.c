#include <ncurses.h>
#include "bplib/bpform.h"
#include "bplib/bpmenu.h"
#include "bplib/bpmessage.h"
#include "chest/session.h"
#include "chest/actions/sign_in.h"
#include "chest/actions/collection.h"

  int
main_menu(Session *session)
{
  int selected;
  int result;

  char *choices[] = {
    "Sign In",
    "Collection",
    "Exit",
    "Help",
  };

  selected = bp_show_menu("Options", choices, 3, 20, 10, 15, 15);

  switch (selected)
  {
    case 0:
      do {
        result = sign_in_form(session);
      } while (result != BP_FORM_OK && result != BP_FORM_CANCEL);
    break;
    case 1:
      collection_menu(session);
    break;
    case 2:
      return -1;
    break;
  }

  return selected;
}

  int
main()
{
  int selected;

  initscr();
  start_color();

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
  Session *session = session_allocate();

  do {
    selected = main_menu(session);
  } while (selected != -1);

  session_destroy(session);
  endwin();

  return 0;
}
