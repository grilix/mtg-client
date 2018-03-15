#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../session.h"
#include "collection.h"

#include "../../bplib/bpmessage.h"
#include "../../bplib/bpmenu.h"

#include "../client/client.h"
#include "../client/collection.h"

  static json_value *
object_key(json_value *value, char *key)
{
  if (value == NULL || (value->type != json_object))
    return NULL;

  int length, x;

  length = value->u.object.length;

  for (x = 0; x < length; x++) {
    if (strcmp(value->u.object.values[x].name, key) == 0)
      return value->u.object.values[x].value;
  }

  return NULL;
}

  static void
show_collection(json_value *value)
{
  int items_count = value->u.array.length;
  json_value *tmp;

  char **items = calloc(items_count + 1, sizeof(char *));

  for (int i = 0; i < items_count; i++)
  {
    tmp = object_key(value->u.array.values[i], "name");

    items[i] = tmp->u.string.ptr;
  }

  bp_show_menu("list", items, items_count, 30, 12, 20, 25);

  free(items);
}


  static void
process_json(ChestResponse *response)
{
  json_value *root = response->json;
  json_value *tmp;

  tmp = object_key(root, "error");

  if (tmp)
  {
    bp_show_message(tmp->u.string.ptr, 10, 10);
    return;
  }

  tmp = object_key(root, "cards");

  if (!tmp)
  {
    bp_show_message("Unknown response structure for collection.", 10, 10);
    return;
  }

  show_collection(tmp);
}

  extern void
collection_menu(Session *session)
{
  ChestResponse *response = chest_get_collection(session);

  if (response->json == NULL)
    bp_show_message("Can't process the response.", 10, 10);
  else
    process_json(response);

  chest_response_destroy(response);
}
