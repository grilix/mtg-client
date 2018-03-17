#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../session.h"
#include "client.h"

  extern ChestResponse *
chest_get_decks(Session *session)
{
  return chest_get(session, "/api/v1/decks");
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

  ChestResponse *response = chest_get(session, path_str);
  free(path_str);

  return response;
}
