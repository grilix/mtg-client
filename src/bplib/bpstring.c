#include <string.h>
#include <stdio.h>
#include <stdlib.h>

  extern int
bp_count_str(const char *str, const char *sub, int max)
{
  int count = 0;

  while (*str && (str = strstr(str, sub)))
  {
    ++count;
    ++str;

    if ((max > 0) && count == max)
      break;
  }

  return count;
}

  extern char**
bp_split_str(const char* str, const char *delim, int max)
{
  int pieces, i, delim_len;
  char **items;
  char *tmp;

  pieces = bp_count_str(str, delim, max - 1) + 1;
  if (pieces == 0)
    return NULL;

  items = (char **)calloc(pieces + 1, sizeof(char *));

  if (items == NULL)
    return NULL;

  i = 0, delim_len = strlen(delim);

  while (i < pieces - 1)
  {
    tmp = strstr(str, delim) + delim_len;

    items[i] = strndup(str, tmp - str - delim_len);
    // If no more memory..
    if (items[i] == NULL)
    {
      // Free whatever we allocated
      pieces = i  - 1;

      for (i = pieces; i >= 0; i--)
        free(items[i]);

      free(items);
      return NULL;
    }

    i++;
    str = tmp;
  }
  items[i] = strdup(str);

  return items;
}

/*
 * Returns the total length of an array of strings adding
 * +extra_item_length+ per item.
 *
 * bpstr_list_len({{"A"}, {"BC"}, {"DEF"}}, 3)
 *   -> (1 + 3) + (2 + 3) + (3 + 3) = 15
 *
 * This is useful if we want to know the length of a string required
 * to include all the items plus a string per element.
 *
 * For example, we want to add a "\r\n" at the end of each element:
 * {{"line 1"}, {"line 2"}}
 *   -> "line 1\r\nline 2\r\n"
 */
  static int
bp_str_list_len(char **list, int extra_item_length)
{
  char **it = list;
  int len = 0;
  while (*it)
    len += strlen(*(it++)) + extra_item_length;

  return len;
}

  extern char *
bp_join_str(char **strings, const char *glue, int glue_end)
{
  int glue_len = strlen(glue), total_len = 0;
  int max_len = bp_str_list_len(strings, glue_len);

  if (glue_end == 0)
    max_len -= glue_len;

  char *buffer = (char *)calloc(1, max_len + 1);
  if (buffer == NULL)
    return NULL;

  char **it = strings;

  while (*it && (total_len < max_len))
  {
    strncat(buffer, *it, max_len - total_len);
    total_len += strlen(*it);

    it++;

    if (it || (glue_end == 1))
    {
      strncat(buffer, glue, max_len - total_len);
      total_len += glue_len;
    }
  }

  return buffer;
}
