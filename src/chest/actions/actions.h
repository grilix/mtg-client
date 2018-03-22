#pragma once

#include "../session.h"
#include "../../json/json.h"
#include "../../bplib/bpmenu.h"

  extern BpMenuItem **
build_card_list(json_value *list);

  extern BpMenuItem **
build_deck_list(json_value *list);

  extern void
collection_menu(Session *session, int x, int y);

  extern void
deck_cards_menu(Session *session, int deck_id, int x, int y);

  extern void
decks_menu(Session *session, int x, int y);

  extern void
sign_in_form(Session *session, int x, int y);
