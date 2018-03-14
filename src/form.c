#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <form.h>

#include "form.h"

#define INPUT_INDEX(i) i * 2
#define LABEL_INDEX(i) (i * 2) + 1

  static char*
trim_whitespaces(char *str);

  static int
form_loop(FORM *form, WINDOW *body_win, int field_count)
{
  int ch, loop = 1;

  keypad(stdscr, TRUE);
  noecho();

  do {
    int current;

    ch = getch();

    switch (ch) {
      case 10:
        current = field_index(current_field(form));

        form_driver(form, REQ_NEXT_FIELD);
        form_driver(form, REQ_END_LINE);

        if (field_index(current_field(form)) < current)
          return FORM_OK;
        break;
      case 27:
        // Alt+x Closes the window
        ch = getch();
        if (ch == 'x')
          return FORM_CANCEL;

        break;

      case KEY_DOWN:
        form_driver(form, REQ_NEXT_FIELD);
        form_driver(form, REQ_END_LINE);
        break;

      case KEY_UP:
        form_driver(form, REQ_PREV_FIELD);
        form_driver(form, REQ_END_LINE);
        break;

      case KEY_LEFT:
        form_driver(form, REQ_PREV_CHAR);
        break;

      case KEY_RIGHT:
        form_driver(form, REQ_NEXT_CHAR);
        break;

        // Delete the char before cursor
      case KEY_BACKSPACE:
      case 127:
        form_driver(form, REQ_DEL_PREV);
        break;

        // Delete the char under the cursor
      case KEY_DC:
        form_driver(form, REQ_DEL_CHAR);
        break;

      default:
        form_driver(form, ch);
        break;
    }

    wrefresh(body_win);
  } while (loop == 1);
}

  static void
form_field_set_value(FormField *field, char *value)
{
  if (field->value != NULL)
    free(field->value);

  field->value = strdup(value);
}

  static void
draw_form_frame(char *title, WINDOW *w, int sizex)
{
  box(w, 0, 0);
  wattron(w, COLOR_PAIR(1));
  mvwprintw(w, 1, 1, "%s", title);
  wattroff(w, COLOR_PAIR(1));

  mvwaddch(w, 2, 0, ACS_LTEE);
  mvwhline(w, 2, 1, ACS_HLINE, sizex - 2);
  mvwaddch(w, 2, sizex - 1, ACS_RTEE);
}

  extern int
show_form(char *title, FormField **form_fields, int field_count, int sizex, int sizey, int x, int y)
{
  FIELD **fields, **field;
  FormField **form_field;

  fields = (FIELD **)calloc(field_count + 1, sizeof(FIELD *));
  field = fields;
  form_field = form_fields;

  for (int i = 0; i < field_count; i++)
  {
    *field = new_field(1, sizex - 8, i, 1, 0, 0);
    set_field_buffer(*field, 0, (*form_field)->value);
    set_field_opts(*field, O_VISIBLE);

    if ((*form_field)->type == FORM_FIELD_TYPE_LABEL)
      field_opts_on(*field, O_AUTOSKIP);
    else
    {
      field_opts_on(*field, O_EDIT | O_ACTIVE);
      set_field_back(*field, A_UNDERLINE);
    }

    if ((*form_field)->type != FORM_FIELD_TYPE_PASSWORD)
      field_opts_on(*field, O_PUBLIC);

    form_field++;
    field++;
  }

  FORM *form = new_form(fields);

  WINDOW *main_win = newwin(sizey, sizex, x, y);
  WINDOW *body_win = derwin(main_win, sizey - 4, sizex - 2, 3, 1);

  draw_form_frame(title, main_win, sizex);

  set_form_win(form, body_win);
  set_form_sub(form, derwin(body_win, sizey - 6, sizex - 4, 1, 1));
  post_form(form);

  refresh();
  wrefresh(main_win);
  wrefresh(body_win);

  int result = form_loop(form, body_win, field_count);

  unpost_form(form);
  free_form(form);
  delwin(body_win);
  delwin(main_win);

  field = fields;
  form_field = form_fields;

  while(*field)
  {
    if (result == FORM_OK)
      form_field_set_value(*form_field,
        trim_whitespaces(field_buffer(*field, 0)));
    free_field(*field);
    form_field++;
    field++;
  }

  free(fields);

  return result;
}

  extern FormField *
form_field_create(enum FormFieldType type, char *value)
{
  FormField *field = (FormField *)malloc(sizeof(FormField));
  field->type = type;
  field->value = NULL;
  form_field_set_value(field, value);
  return field;
}

  extern void
form_field_destroy(FormField *field)
{
  free(field->value);
  free(field);
}

/*
 * This is useful because ncurses fill fields blanks with spaces.
 */
  static char*
trim_whitespaces(char *str)
{
  char *end;

  // trim leading space
  while (isspace(*str))
    str++;

  if (*str == 0) // all spaces?
    return str;

  // trim trailing space
  end = str + strnlen(str, 128) - 1;

  while (end > str && isspace(*end))
    end--;

  // write new null terminator
  *(end + 1) = '\0';

  return str;
}
