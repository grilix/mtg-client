#ifndef CHEST_ACTIONS_ACTIONS_H
#define CHEST_ACTIONS_ACTIONS_H

#include "../session.h"
#include "../../json/json.h"
#include "../../bplib/bpmenu.h"

  extern BpMenuItem **
build_card_list(json_value *list, int list_items);

  extern void
collection_menu(Session *session, int x, int y);

  extern void
deck_cards_menu(Session *session, int deck_id, int x, int y);

  extern void
decks_menu(Session *session, int x, int y);

  extern void
sign_in_form(Session *session, int x, int y);

#endif
