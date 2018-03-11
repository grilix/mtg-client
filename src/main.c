#include <ncurses.h>
#include "menu.h"
#include "actions/sign_in.h"

  int
main_menu()
{
  char *choices[] = {
    "Sign In",
    "Exit",
    "Help",
  };

  return show_menu("Options", choices, 3, 20, 10, 15, 15);
}

  int
main()
{
  int loop = 1;

  initscr();
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);

  do {
    int selected = main_menu();

    switch (selected)
    {
      case 0:
        sign_in_start();
      break;
      case 1:
        loop = 0;
      break;
    }
  } while (loop);

  endwin();

  return 0;
}
