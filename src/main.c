#include <ncurses.h>
#include "menu.h"
#include "actions/sign_in.h"
#include "actions/collection.h"

  int
main_menu()
{
  int selected;

  char *choices[] = {
    "Sign In",
    "Collection",
    "Exit",
    "Help",
  };

  selected = show_menu("Options", choices, 3, 20, 10, 15, 15);

  switch (selected)
  {
    case 0:
      sign_in_start();
    break;
    case 1:
      collection_menu();
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

  do {
    selected = main_menu();
  } while (selected != -1);

  endwin();

  return 0;
}
