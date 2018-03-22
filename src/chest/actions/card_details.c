#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../../bplib/bpmenu.h"
#include "../../bplib/bpmessage.h"
#include "../client/client.h"
#include "../session.h"
#include "actions.h"

static void
show_details(BpWindow *details, json_value *card)
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

extern void
show_card_details(Session *session, BpWindow *parent, int card_id)
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
    show_details(details, response->json_root);

  bp_window_destroy_clear(details);
  chest_response_destroy(response);
}
