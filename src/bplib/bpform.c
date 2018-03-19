#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <form.h>

#include "bpwindow.h"
#include "bpform.h"

/*
 * This file contains code copied from/based on:
 *   For +trim_whitespaces+, and form driver:
 *     https://gist.github.com/alan-mushi/c8a6f34d1df18574f643
 */

  static char*
trim_whitespaces(char *str);

  extern void
bp_form_driver(BpForm *form, int ch)
{
  int current = field_index(current_field(form->_form));

  switch (ch) {
    case 10:
      form_driver(form->_form, REQ_NEXT_FIELD);

      if (field_index(current_field(form->_form)) < current)
      {
        form_driver(form->_form, REQ_PREV_FIELD);
        bp_form_sync_input(form);
        form->status = BP_WINDOW_STATUS_PROCESSING;
      }
      else
        form_driver(form->_form, REQ_END_LINE);

      break;

    case KEY_DOWN:
      form_driver(form->_form, REQ_NEXT_FIELD);

      if (field_index(current_field(form->_form)) < current)
        form_driver(form->_form, REQ_PREV_FIELD);
      form_driver(form->_form, REQ_END_LINE);

      break;

    case KEY_UP:
      form_driver(form->_form, REQ_PREV_FIELD);

      if (field_index(current_field(form->_form)) > current)
        form_driver(form->_form, REQ_NEXT_FIELD);
      else
        form_driver(form->_form, REQ_END_LINE);

      break;

    case KEY_LEFT:
      form_driver(form->_form, REQ_PREV_CHAR);
      break;

    case KEY_RIGHT:
      form_driver(form->_form, REQ_NEXT_CHAR);
      break;

      // Delete the char before cursor
    case KEY_BACKSPACE:
    case 127:
      // TODO: Don't move to previous field when no more
      // characters to delete.
      form_driver(form->_form, REQ_DEL_PREV);
      break;

      // Delete the char under the cursor
    case KEY_DC:
      form_driver(form->_form, REQ_DEL_CHAR);
      break;

    default:
      form_driver(form->_form, ch);
      break;
  }

  wrefresh(form->_body_win);
}

  extern void
bp_form_loop(BpForm *form)
{
  int ch;

  keypad(form->_main_win->_window, TRUE);
  noecho();

  form->status = form->_main_win->status = BP_WINDOW_STATUS_LOOPING;

  do {
    ch = bp_window_getch(form->_main_win);

    if (!bp_window_driver(form->_main_win, ch))
      bp_form_driver(form, ch);

    // If window is done, give up.
    if (form->_main_win->status != BP_WINDOW_STATUS_LOOPING)
    {
      if (form->status == BP_WINDOW_STATUS_LOOPING)
        form->status = BP_WINDOW_STATUS_COMPLETE;
    }


  } while(form->status == BP_WINDOW_STATUS_LOOPING);
}
  extern void
bp_form_field_set_value(BpFormField *field, char *value)
{
  if (field->value != NULL)
    free(field->value);

  field->value = strdup(value);
}

  static void
bp_form_create_fields(BpForm *form,
                      BpFormField **form_fields,
                      int field_count)
{
  FIELD **field;
  BpFormField **form_field;

  form->_fields = (FIELD **)calloc(field_count + 1, sizeof(FIELD *));

  field = form->_fields;
  form->_form_fields = form_field = form_fields;

  for (int i = 0; i < field_count; i++)
  {
    *field = new_field(1, form->sizex - 8, i, 1, 0, 0);
    set_field_buffer(*field, 0, (*form_field)->value);
    set_field_opts(*field, O_VISIBLE);

    if ((*form_field)->type == BP_FORM_FIELD_TYPE_LABEL)
      field_opts_on(*field, O_AUTOSKIP);
    else
    {
      field_opts_on(*field, O_EDIT | O_ACTIVE);
      set_field_back(*field, A_UNDERLINE);
    }

    if ((*form_field)->type != BP_FORM_FIELD_TYPE_PASSWORD)
      field_opts_on(*field, O_PUBLIC);

    form_field++;
    field++;
  }
}

  static void
bp_form_destroy_fields(BpForm *form)
{
  FIELD **field = form->_fields;

  while (*field)
  {
    free_field(*field);
    field++;
  }

  free(form->_fields);
}

  static void
display_form(BpForm *form)
{
  form->_form = new_form(form->_fields);

  set_form_win(form->_form, form->_body_win);
  set_form_sub(form->_form,
               derwin(form->_body_win,
                      form->sizey - 6, form->sizex - 4, 1, 1));

  post_form(form->_form);

  //refresh();
  wrefresh(form->_main_win->_window);
  wrefresh(form->_body_win);
}

  extern BpForm *
bp_form_create(char *title, BpFormField **form_fields, int field_count,
              int sizex, int sizey, int x, int y)
{
  BpForm *form = (BpForm *)malloc(sizeof(BpForm));

  form->sizex = sizex;
  form->sizey = sizey;
  form->x = x;
  form->y = y;

  form->_main_win = bp_window_create_frame(title, sizex, sizey, x, y);
  form->_body_win = derwin(form->_main_win->_window,
                           sizey - 4, sizex - 2, 3, 1);

  bp_form_create_fields(form, form_fields, field_count);

  display_form(form);

  return form;
}

  extern void
bp_form_sync_input(BpForm *form)
{
  FIELD **field = form->_fields;
  BpFormField **form_field = form->_form_fields;

  while(*field)
  {
    bp_form_field_set_value(*form_field,
      trim_whitespaces(field_buffer(*field, 0)));
    form_field++;
    field++;
  }
}

  extern void
bp_form_update_fields(BpForm *form, BpFormField **fields, int field_count)
{
  unpost_form(form->_form);
  free_form(form->_form);
  bp_form_destroy_fields(form);

  bp_form_create_fields(form, fields, field_count);

  display_form(form);
}

  extern void
bp_form_destroy(BpForm *form)
{
  unpost_form(form->_form);
  free_form(form->_form);
  delwin(form->_body_win);
  bp_window_destroy_clear(form->_main_win);

  bp_form_destroy_fields(form);

  free(form);
}

  extern BpFormField *
bp_form_field_create(enum BpFormFieldType type, char *value)
{
  BpFormField *field = (BpFormField *)malloc(sizeof(BpFormField));

  field->type = type;
  field->value = NULL;
  bp_form_field_set_value(field, value);

  return field;
}

  extern void
bp_form_field_destroy(BpFormField *field)
{
  free(field->value);
  free(field);
}

/*
 * This is useful because ncurses fill fields blanks with spaces.
 *
 */
  static char*
trim_whitespaces(char *str)
{
  char *end;

  while (isspace(*str))
    str++;

  if (*str == 0)
    return str;

  end = str + strnlen(str, 128) - 1;

  while (end > str && isspace(*end))
    end--;

  *(end + 1) = '\0';

  return str;
}
