#include "../session.h"
#include "actions.h"
#include "../client/client.h"
#include "../../json/json.h"
#include "../../bplib/bpmenu.h"

  extern BpMenuItem **
build_card_list(json_value *list, int list_items)
{
  json_value *tmp;

  BpMenuItem **items =
    (BpMenuItem **)calloc(list_items, sizeof(BpMenuItem *));

  for (int i = 0; i < list_items; i++)
  {
    tmp = json_object_key(list->u.array.values[i], "card_name");

    items[i] = (BpMenuItem *)malloc(sizeof(BpMenuItem));
    items[i]->title = tmp->u.string.ptr;

    tmp = json_object_key(list->u.array.values[i], "card_id");

    if (tmp != NULL)
      items[i]->value = tmp->u.integer;
    else
      items[i]->value = -1;
  }

  return items;
}
