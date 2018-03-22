#include <assert.h>
#include <string.h>

#include "../../bplib/bpmenu.h"
#include "../../json/json.h"
#include "../client/client.h"
#include "../session.h"
#include "actions.h"

static char **
allocate_string_list(int count, int size)
{
  char **strings = (char **)calloc(count, sizeof(char *));

  if (strings == NULL)
    return NULL;

  strings[0] = (char *)calloc(count, size * sizeof(char));
  if (strings[0] == NULL)
  {
    free(strings);
    strings = NULL;
  }

  return strings;
}

static void
destroy_items_list(BpMenuItem **items)
{
  free(items[0]->title);
  bp_menu_item_destroy_list(items);
}

extern void
destroy_card_list(BpMenuItem **items)
{
  destroy_items_list(items);
}

extern void
destroy_deck_list(BpMenuItem **items)
{
  destroy_items_list(items);
}

extern BpMenuItem **
build_card_list(json_value *list, int max_size)
{
  json_value *tmp;
  int list_items = list->u.array.length;
  BpMenuItem **items = bp_menu_item_create_list(list_items);

  int count, i;
  char *format = "%3dx %30s %s";
  char *mana_cost;

  char **titles = allocate_string_list(list_items, max_size);
  if (titles == NULL)
  {
    destroy_items_list(items);
    return NULL;
  }

  for (i = 0; i < list_items; i++)
  {
    items[i] = &items[0][i];
    titles[i] = titles[0] + (i * max_size);

    tmp = json_object_key(list->u.array.values[i], "card_count");
    count = (tmp == NULL) ? 0 : tmp->u.integer;

    tmp = json_object_key(list->u.array.values[i], "card_id");
    items[i]->value = (tmp == NULL) ? -1 : tmp->u.integer;

    tmp = json_object_key(list->u.array.values[i], "card_mana_cost");
    mana_cost = (tmp == NULL) ? "" : tmp->u.string.ptr;

    tmp = json_object_key(list->u.array.values[i], "card_name");

    if (tmp != NULL)
      snprintf(titles[i], max_size, format, count, tmp->u.string.ptr, mana_cost);
    else
      snprintf(titles[i], max_size, format, count, "<No name>", mana_cost);

    items[i]->title = titles[i];
  }

  free(titles);
  return items;
}

extern BpMenuItem **
build_deck_list(json_value *list, int max_size)
{
  json_value *tmp;
  int list_items = list->u.array.length;

  BpMenuItem **items = bp_menu_item_create_list(list_items);

  char **titles = allocate_string_list(list_items, max_size);
  if (titles == NULL)
  {
    destroy_items_list(items);
    return NULL;
  }

  for (int i = 0; i < list_items; i++)
  {
    items[i] = items[0] + i;
    titles[i] = titles[0] + (i * max_size);

    tmp = json_object_key(list->u.array.values[i], "deck_id");
    items[i]->value = tmp->u.integer;

    tmp = json_object_key(list->u.array.values[i], "deck_name");
    if (tmp != NULL)
      snprintf(titles[i], max_size, "%s", tmp->u.string.ptr);
    else
      snprintf(titles[i], max_size, "<No name>");

    items[i]->title = titles[i];
  }

  free(titles);
  return items;
}
