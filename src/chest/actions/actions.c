#include <string.h>
#include "../session.h"
#include "actions.h"
#include "../client/client.h"
#include "../../json/json.h"
#include "../../bplib/bpmenu.h"

  extern BpMenuItem **
build_card_list(json_value *list)
{
  json_value *tmp;
  int list_items = list->u.array.length;

  int value;
  int count;

  BpMenuItem **items =
    (BpMenuItem **)calloc(list_items, sizeof(BpMenuItem *));

  char title[100];

  for (int i = 0; i < list_items; i++)
  {
    tmp = json_object_key(list->u.array.values[i], "card_count");
    count = (tmp == NULL) ? 0 : tmp->u.integer;

    tmp = json_object_key(list->u.array.values[i], "card_id");
    value = (tmp == NULL) ? -1 : tmp->u.integer;

    tmp = json_object_key(list->u.array.values[i], "card_name");

    if (tmp)
      snprintf(title, 100, "%2d - %s", count, tmp->u.string.ptr);
    else
      snprintf(title, 100, "%2d - %s", count, "<No name>");

    items[i] = bp_menu_item_create(title, value);
  }

  return items;
}

  extern BpMenuItem **
build_deck_list(json_value *list)
{
  json_value *tmp;
  int list_items = list->u.array.length;
  int tmp_value;

  BpMenuItem **items =
    (BpMenuItem **)calloc(list_items, sizeof(BpMenuItem *));

  for (int i = 0; i < list_items; i++)
  {
    tmp = json_object_key(list->u.array.values[i], "deck_id");
    tmp_value = tmp->u.integer;

    tmp = json_object_key(list->u.array.values[i], "deck_name");

    items[i] = bp_menu_item_create(tmp->u.string.ptr, tmp_value);
  }

  return items;
}
