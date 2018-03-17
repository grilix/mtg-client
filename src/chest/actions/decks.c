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
  json_value *tmp;

  if (items_count == 0)
  {
    bp_show_message("No decks found", 20, 22);
    return;
  }

  BpMenuItem **items =
    (BpMenuItem **)calloc(items_count, sizeof(BpMenuItem *));

  for (i = 0; i < items_count; i++)
  {
    tmp = json_object_key(value->u.array.values[i], "name");

    items[i] = (BpMenuItem *)malloc(sizeof(BpMenuItem));
    items[i]->title = tmp->u.string.ptr;

    tmp = json_object_key(value->u.array.values[i], "id");
    items[i]->value = tmp->u.integer;
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
        deck_cards_menu(session, menu->selected->value, 15, 15);
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
process_json(Session *session, BpMenu *menu, ChestResponse *response)
{
  json_value *root = response->json;
  json_value *tmp;

  tmp = json_object_key(root, "error");

  if (tmp)
  {
    bp_show_message(tmp->u.string.ptr, 10, 10);
    return;
  }

  tmp = json_object_key(root, "decks");

  if (!tmp)
  {
    bp_show_message("Unknown response structure for decks.", 10, 10);
    return;
  }

  show_decks(session, menu, tmp);
}

  extern void
decks_menu(Session *session, int x, int y)
{
  BpMenu *menu = bp_menu_create("Decks", NULL, 0, 40, 20, x, y);

  ChestResponse *response = chest_get_decks(session);

  if (response->json == NULL)
    bp_show_message("Can't process the response.", x, y);
  else
    process_json(session, menu, response);

  bp_menu_destroy_clear(menu);
  chest_response_destroy(response);
}
