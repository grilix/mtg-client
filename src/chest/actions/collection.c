#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../bplib/bpmenu.h"
#include "../../bplib/bpmessage.h"
#include "../client/client.h"
#include "../session.h"
#include "actions.h"

static void
show_collection(Session *session, BpMenu *menu, json_value *value)
{
  size_t items_count = value->u.array.length;

  if (items_count == 0)
  {
    bp_window_show_message(menu->_window, "No cards in your collection");
    return;
  }

  BpMenuItem **items = build_card_list(value, menu->_window->sizex - 2);

  if (items == NULL)
    return;

  bp_menu_set_items(menu, items, items_count);

  bp_menu_loop_start(menu);

  do
  {
    bp_menu_loop_handler(menu);

    if (menu->status != BP_WINDOW_STATUS_LOOPING &&
        menu->_window->status == BP_WINDOW_STATUS_LOOPING)
    {
      if ((menu->selected != NULL) && (menu->selected->value > 0))
      {
        show_card_details(session, menu->_window, menu->selected->value);
        bp_window_refresh(menu->_window);
      }
      menu->status = BP_WINDOW_STATUS_LOOPING;
    }
  } while (menu->status == BP_WINDOW_STATUS_LOOPING);

  destroy_card_list(items);
}

extern void
collection_menu(Session *session, int x, int y)
{
  BpMenu *menu = bp_menu_create("Collection", NULL, 0, 60, 20, x, y);

  ChestResponse *response = chest_get_collection(session);

  if (response == NULL)
  {
    bp_window_show_message(menu->_window, "Can't connect to the server");
    bp_menu_destroy_clear(menu);
    return;
  }

  if (response->error != NULL)
    bp_window_show_message(menu->_window, response->error);
  else
    show_collection(session, menu, response->json_root);

  bp_menu_destroy_clear(menu);
  chest_response_destroy(response);
}
