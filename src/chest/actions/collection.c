#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../session.h"
#include "actions.h"

#include "../../bplib/bpmessage.h"
#include "../../bplib/bpmenu.h"

#include "../client/client.h"

  static void
show_details(BpMenu *menu)
{
  BpWindow *details = bp_window_create_frame("Details", 30, 10, 45, 15);

  mvwprintw(details->_window, 3, 2, "%s", menu->selected->title);
  wrefresh(details->_window);

  bp_window_getch(menu->_window);
  bp_window_destroy_clear(details);
}

  static void
show_collection(BpMenu *menu, json_value *value)
{
  int i;
  int items_count = value->u.array.length;

  if (items_count == 0)
  {
    bp_window_show_message(menu->_window, "No cards in your collection");
    return;
  }

  BpMenuItem **items = build_card_list(value);

  bp_menu_set_items(menu, items, items_count);

  bp_menu_loop_start(menu);

  do {
    bp_menu_loop_handler(menu);

    if (menu->status != BP_WINDOW_STATUS_LOOPING &&
      menu->_window->status == BP_WINDOW_STATUS_LOOPING)
    {
      if (menu->selected != NULL)
      {
        show_details(menu);
        bp_window_refresh(menu->_window);
      }
      menu->status = BP_WINDOW_STATUS_LOOPING;
    }
 } while (menu->status == BP_WINDOW_STATUS_LOOPING);

  for (i = 0; i < items_count; i++)
    bp_menu_item_destroy(items[i]);

  free(items);
}

  extern void
collection_menu(Session *session, int x, int y)
{
  BpMenu *menu = bp_menu_create("Collection", NULL, 0, 40, 20, x, y);

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
    show_collection(menu, response->json_root);

  bp_menu_destroy_clear(menu);
  chest_response_destroy(response);
}
