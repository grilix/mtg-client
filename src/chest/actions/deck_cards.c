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
  json_value *tmp;

  if (items_count == 0)
  {
    bp_show_message("No cards in this deck", 20, 22);
    return;
  }

  BpMenuItem **items =
    (BpMenuItem **)calloc(items_count, sizeof(BpMenuItem *));

  for (i = 0; i < items_count; i++)
  {
    tmp = json_object_key(value->u.array.values[i], "name");

    items[i] = (BpMenuItem *)malloc(sizeof(BpMenuItem));
    items[i]->title = tmp->u.string.ptr;
  }

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
    free(items[i]);

  free(items);
}


  static void
process_json(BpMenu *menu, ChestResponse *response)
{
  json_value *root = response->json;
  json_value *tmp;

  tmp = json_object_key(root, "error");

  if (tmp)
  {
    bp_show_message(tmp->u.string.ptr, 10, 10);
    return;
  }

  tmp = json_object_key(root, "cards");

  if (!tmp)
  {
    bp_show_message("Unknown response structure for collection.", 10, 10);
    return;
  }

  show_collection(menu, tmp);
}

  extern void
deck_cards_menu(Session *session, int deck_id, int x, int y)
{
  BpMenu *menu = bp_menu_create("Deck cards", NULL, 0, 40, 20, x, y);

  ChestResponse *response = chest_get_deck_cards(session, deck_id);

  if (response->json == NULL)
    bp_show_message("Can't process the response.", x, x);
  else
    process_json(menu, response);

  bp_menu_destroy_clear(menu);
  chest_response_destroy(response);
}
