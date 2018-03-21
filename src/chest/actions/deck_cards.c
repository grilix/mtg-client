#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../session.h"
#include "actions.h"

#include "../../bplib/bpmessage.h"
#include "../../bplib/bpmenu.h"

#include "../client/client.h"

static BpMenuItem
  menu_details = {.value = 0, .title = "Details"},
  menu_alternatives = {.value = 0, .title = "Alternatives"};

  static void
show_card_details(BpWindow *details, json_value *printing)
{
  json_value *tmp = json_object_key(printing, "card_name");

  mvwprintw(details->_window, 3, 2, "Name: %s", tmp->u.string.ptr);
  wrefresh(details->_window);

  bp_window_getch(details);
}

  static void
show_details(Session *session, BpWindow *parent, int deck_id, int card_id)
{
  BpWindow *details = bp_window_create_frame("Details", 30, 10,
                                             parent->x + 3, parent->y + 3);

  ChestResponse *response = chest_get_deck_card_details(
    session, deck_id, card_id
  );

  if (response == NULL)
  {
    bp_window_show_message(parent, "Can't connect to the server");
    bp_window_destroy_clear(details);
    return;
  }

  if (response->error != NULL)
    bp_window_show_message(parent, response->error);
  else
    show_card_details(details, response->json_root);

  bp_window_destroy_clear(details);
  chest_response_destroy(response);
}

  static void
show_alternatives_list(Session *session, BpMenu *menu,
                       int deck_id, json_value *value)
{
  int i;
  int items_count = value->u.array.length;

  if (items_count == 0)
  {
    bp_show_message("No cards in this deck", 20, 22);
    return;
  }

  BpMenuItem **items = build_card_list(value, value->u.array.length);

  bp_menu_set_items(menu, items, items_count);

  bp_menu_loop_start(menu);

  do {
    bp_menu_loop_handler(menu);
 } while (menu->status == BP_WINDOW_STATUS_LOOPING);

  for (i = 0; i < items_count; i++)
    free(items[i]);

  free(items);
}
  static void
show_card_alternatives(Session *session, BpWindow *parent,
                       int deck_id, int card_id)
{
  BpMenu *menu = bp_menu_create("Alternatives", NULL, 0, 40, 20,
                                parent->x + 3, parent->y + 3);

  ChestResponse *response = chest_get_deck_card_alternatives(
    session, deck_id, card_id
  );

  if (response == NULL)
  {
    bp_window_show_message(menu->_window, "Can't connect to the server");
    bp_menu_destroy_clear(menu);
    return;
  }

  if (response->error != NULL)
    bp_window_show_message(menu->_window, response->error);
  else
    show_alternatives_list(session, menu, deck_id, response->json_root);

  bp_menu_destroy_clear(menu);
  chest_response_destroy(response);
}

  static void
show_card_menu(Session *session, BpMenu *cards, int deck_id)
{
  int card_id = cards->selected->value;

  BpMenuItem *selected = NULL;
  BpMenuItem **choices =
    (BpMenuItem **)calloc(2, sizeof(BpMenuItem *));
  choices[0] = &menu_details;
  choices[1] = &menu_alternatives;

  BpMenu *menu = bp_menu_create(cards->selected->title, choices, 2, 20, 10,
                                cards->_window->x + 3, cards->_window->y + 3);

  bp_menu_loop(menu);

  if (menu->_window->status == BP_WINDOW_STATUS_LOOPING)
  {
    selected = menu->selected;

    if (selected == &menu_details)
      show_details(session, menu->_window, deck_id, card_id);
    else if (selected == &menu_alternatives)
      show_card_alternatives(session, menu->_window, deck_id, card_id);
  }

  free(choices);
  bp_menu_destroy_clear(menu);
}

  static void
show_deck(Session *session, BpMenu *menu, int deck_id, json_value *value)
{
  int i;
  int items_count = value->u.array.length;
  json_value *tmp;

  if (items_count == 0)
  {
    bp_show_message("No cards in this deck", 20, 22);
    return;
  }

  BpMenuItem **items = build_card_list(value, value->u.array.length);

  bp_menu_set_items(menu, items, items_count);

  bp_menu_loop_start(menu);

  do {
    bp_menu_loop_handler(menu);

    if (menu->status != BP_WINDOW_STATUS_LOOPING &&
      menu->_window->status == BP_WINDOW_STATUS_LOOPING)
    {
      if (menu->selected != NULL)
      {
        if (menu->selected->value > 0)
        {
          show_card_menu(session, menu, deck_id);
        }
        else
          bp_window_show_message(menu->_window, "Can't load details");

        bp_window_refresh(menu->_window);
      }
      menu->status = BP_WINDOW_STATUS_LOOPING;
    }
 } while (menu->status == BP_WINDOW_STATUS_LOOPING);

  for (i = 0; i < items_count; i++)
    free(items[i]);

  free(items);
}

  extern void
deck_cards_menu(Session *session, int deck_id, int x, int y)
{
  BpMenu *menu = bp_menu_create("Deck cards", NULL, 0, 40, 20, x, y);

  ChestResponse *response = chest_get_deck_cards(session, deck_id);

  if (response == NULL)
  {
    bp_window_show_message(menu->_window, "Can't connect to the server");
    bp_menu_destroy_clear(menu);
    return;
  }

  if (response->error != NULL)
    bp_window_show_message(menu->_window, response->error);
  else
    show_deck(session, menu, deck_id, response->json_root);

  bp_menu_destroy_clear(menu);
  chest_response_destroy(response);
}
