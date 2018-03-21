#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../session.h"
#include "client.h"

  extern ChestResponse *
chest_get_decks(Session *session)
{
  return chest_get(session, "/api/v1/decks", "decks");
}

  extern ChestResponse *
chest_get_deck_cards(Session *session, int deck_id)
{
  char *path_format = "/api/v1/decks/%d";
  int max_str_size = strlen(path_format) + 10;
  char *path_str;
  path_str = calloc(max_str_size + 1, sizeof(char));

  int result = snprintf(path_str, max_str_size, path_format, deck_id);
  // If this fails, we need to allocate space for bigger ids
  assert(result < max_str_size);

  ChestResponse *response = chest_get(session, path_str, "cards");
  free(path_str);

  return response;
}

  extern ChestResponse *
chest_get_deck_card_alternatives(Session *session, int deck_id,
                                 int card_id)
{
  char *path_format = "/api/v1/decks/%d/cards/%d/alternatives";
  int max_str_size = strlen(path_format) + 20;
  char *path_str;
  path_str = calloc(max_str_size + 1, sizeof(char));

  snprintf(path_str, max_str_size, path_format, deck_id, card_id);

  ChestResponse *response = chest_get(session, path_str, "cards");
  free(path_str);

  return response;
}

  extern ChestResponse *
chest_get_deck_card_details(Session *session, int deck_id,
                            int user_printing_id)
{
  char *path_format = "/api/v1/decks/%d/cards/%d";
  int max_str_size = strlen(path_format) + 20;
  char *path_str;
  path_str = calloc(max_str_size + 1, sizeof(char));

  snprintf(path_str, max_str_size, path_format, deck_id, user_printing_id);

  ChestResponse *response = chest_get(session, path_str, "details");
  free(path_str);

  return response;

}
