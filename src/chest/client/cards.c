#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../session.h"
#include "client.h"

extern ChestResponse *
chest_get_card_details(Session *session, int user_printing_id)
{
  char *path_format = "/api/v1/cards/%d";
  int max_str_size = strlen(path_format) + 20;
  char *path_str;

  path_str = calloc(max_str_size + 1, sizeof(char));

  snprintf(path_str, max_str_size, path_format, user_printing_id);

  ChestResponse *response = chest_get(session, path_str, "details");
  free(path_str);

  return response;
}
