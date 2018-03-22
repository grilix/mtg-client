#pragma once

#include "../../bplib/bpmenu.h"
#include "../../json/json.h"
#include "../session.h"

extern BpMenuItem **
build_card_list(json_value *list, int max_size);

extern void
destroy_card_list(BpMenuItem **items);

extern void
destroy_deck_list(BpMenuItem **items);

extern BpMenuItem **
build_deck_list(json_value *list, int max_size);

extern void
collection_menu(Session *session, int x, int y);

extern void
deck_cards_menu(Session *session, int deck_id, int x, int y);

extern void
decks_menu(Session *session, int x, int y);

extern void
sign_in_form(Session *session, int x, int y);

extern void
show_card_details(Session *session, BpWindow *parent, int card_id);
