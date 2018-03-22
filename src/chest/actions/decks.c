#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../session.h"
#include "actions.h"

#include "../../bplib/bpmessage.h"
#include "../../bplib/bpmenu.h"

#include "../client/client.h"

  static void
show_decks(Session *session, BpMenu *menu, json_value *value)
{
  int i;
  int items_count = value->u.array.length;

  if (items_count == 0)
  {
    bp_window_show_message(menu->_window, "No decks found");
    return;
  }

  BpMenuItem **items = build_deck_list(value, menu->_window->sizex - 2);

  bp_menu_set_items(menu, items, items_count);

  bp_menu_loop_start(menu);

  do {
    bp_menu_loop_handler(menu);

    if (menu->status != BP_WINDOW_STATUS_LOOPING &&
      menu->_window->status == BP_WINDOW_STATUS_LOOPING)
    {
      if (menu->selected != NULL)
      {
        deck_cards_menu(session, menu->selected->value,
                        menu->_window->x + 2, menu->_window->y + 2);
        bp_window_refresh(menu->_window);
      }

      menu->status = BP_WINDOW_STATUS_LOOPING;
    }
 } while (menu->status == BP_WINDOW_STATUS_LOOPING);

  destroy_deck_list(items);
}

  extern void
decks_menu(Session *session, int x, int y)
{
  BpMenu *menu = bp_menu_create("Decks", NULL, 0, 40, 20, x, y);

  ChestResponse *response = chest_get_decks(session);

  if (response == NULL)
  {
    bp_window_show_message(menu->_window, "Can't connect to the server");
    bp_menu_destroy_clear(menu);
    return;
  }

  if (response->error != NULL)
    bp_window_show_message(menu->_window, response->error);
  else
    show_decks(session, menu, response->json_root);

  bp_menu_destroy_clear(menu);
  chest_response_destroy(response);
}
