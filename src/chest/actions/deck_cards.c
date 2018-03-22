#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../bplib/bpmenu.h"
#include "../../bplib/bpmessage.h"
#include "../client/client.h"
#include "../session.h"
#include "actions.h"

static BpMenuItem menu_details = {.value = 0, .title = "Details"},
                  menu_alternatives = {.value = 0, .title = "Alternatives"};

static void
show_card_details(BpWindow *details, json_value *card)
{
  WINDOW *w;
  int text_y = 3 + // Counting the title and border
               2 + // Counting the card name line and margin
               2   // Counting type line and margin
      ;
  int text_sizey = details->sizey - text_y - 1;
  char buffer[20];

  json_value *tmp, *tmp2;

  tmp = json_object_key(card, "card_name");
  if (tmp == NULL)
    mvwprintw(details->_window, 3, 2, "<No name>");
  else
    mvwprintw(details->_window, 3, 2, "%s", tmp->u.string.ptr);

  tmp = json_object_key(card, "card_mana_cost");
  if ((tmp != NULL) && (tmp->u.string.ptr != NULL))
  {
    snprintf(buffer, 20, "%s", tmp->u.string.ptr);
    mvwprintw(
        details->_window, 3, details->sizex - 2 - strlen(buffer), "%s", buffer);
  }

  tmp = json_object_key(card, "card_power");
  if ((tmp != NULL) && (tmp->u.string.ptr != NULL))
  {
    tmp2 = json_object_key(card, "card_toughness");
    snprintf(buffer, 5, "%s/%s", tmp->u.string.ptr, tmp2->u.string.ptr);
    mvwprintw(
        details->_window, 5, details->sizex - 2 - strlen(buffer), "%s", buffer);
  }

  tmp = json_object_key(card, "card_type");
  if (tmp != NULL)
    mvwprintw(details->_window, 5, 2, "%s", tmp->u.string.ptr);

  tmp = json_object_key(card, "card_text");

  if (tmp != NULL)
  {
    // Hack for wrapping text
    w = derwin(details->_window, text_sizey, details->sizex - 2, text_y, 1);
    mvwprintw(w, 0, 0, "%s", tmp->u.string.ptr);
    delwin(w);
  }

  wrefresh(details->_window);

  bp_window_getch(details);
}

static void
show_details(Session *session, BpWindow *parent, int card_id)
{
  BpWindow *details =
      bp_window_create_frame("Details", 40, 17, parent->x + 3, parent->y + 3);

  ChestResponse *response = chest_get_card_details(session, card_id);

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
show_alternatives_list(Session *session, BpMenu *menu, int deck_id,
                       json_value *value)
{
  int i;
  int items_count = value->u.array.length;

  if (items_count == 0)
  {
    bp_show_message("No cards in this deck", 20, 22);
    return;
  }

  BpMenuItem **items = build_card_list(value);

  bp_menu_set_items(menu, items, items_count);

  bp_menu_loop_start(menu);

  do
  {
    bp_menu_loop_handler(menu);

    if (menu->status != BP_WINDOW_STATUS_LOOPING &&
        menu->_window->status == BP_WINDOW_STATUS_LOOPING)
    {
      if (menu->selected != NULL)
      {
        if (menu->selected->value > 0)
        {
          show_details(session, menu->_window, menu->selected->value);
        }
        else
          bp_window_show_message(menu->_window, "Can't load details");

        bp_window_refresh(menu->_window);
      }
      menu->status = BP_WINDOW_STATUS_LOOPING;
    }
  } while (menu->status == BP_WINDOW_STATUS_LOOPING);

  for (i = 0; i < items_count; i++)
    bp_menu_item_destroy(items[i]);

  free(items);
}
static void
show_card_alternatives(Session *session, BpWindow *parent, int deck_id,
                       int card_id)
{
  BpMenu *menu = bp_menu_create(
      "Alternatives", NULL, 0, 40, 20, parent->x + 3, parent->y + 3);

  ChestResponse *response =
      chest_get_deck_card_alternatives(session, deck_id, card_id);

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
  BpMenuItem **choices = (BpMenuItem **)calloc(2, sizeof(BpMenuItem *));
  choices[0] = &menu_details;
  choices[1] = &menu_alternatives;

  BpMenu *menu = bp_menu_create(cards->selected->title,
                                choices,
                                2,
                                20,
                                10,
                                cards->_window->x + 3,
                                cards->_window->y + 3);

  bp_menu_loop(menu);

  if (menu->_window->status == BP_WINDOW_STATUS_LOOPING)
  {
    selected = menu->selected;

    if (selected == &menu_details)
      show_details(session, menu->_window, card_id);
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

  if (items_count == 0)
  {
    bp_show_message("No cards in this deck", 20, 22);
    return;
  }

  BpMenuItem **items = build_card_list(value);

  bp_menu_set_items(menu, items, items_count);

  bp_menu_loop_start(menu);

  do
  {
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
    bp_menu_item_destroy(items[i]);

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
