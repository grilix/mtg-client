#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "../../bplib/bpform.h"
#include "../../bplib/bpmessage.h"
#include "../session.h"
#include "../client/client.h"

#include "actions.h"

  extern void
sign_in_form(Session *session, int x, int y)
{
  BpFormField **fields;

  fields = (BpFormField **)calloc(4, sizeof(BpFormField *));
  fields[0] = bp_form_field_create(BP_FORM_FIELD_TYPE_LABEL, "Name");
  fields[1] = bp_form_field_create(BP_FORM_FIELD_TYPE_INPUT, "");
  fields[2] = bp_form_field_create(BP_FORM_FIELD_TYPE_LABEL, "Password");
  fields[3] = bp_form_field_create(BP_FORM_FIELD_TYPE_PASSWORD, "");

  BpForm *form = bp_form_create("Sign in", fields, 4, 40, 10, x, y);

  do
  {
    bp_form_loop(form);

    if (form->status == BP_WINDOW_STATUS_PROCESSING)
    {
      bp_form_sync_input(form);

      if (sign_in(session, fields[1]->value, fields[3]->value) == 0)
        form->status = BP_WINDOW_STATUS_COMPLETE;
      else
      {
        bp_show_message("Login failed.", x + 5, y + 2);
        bp_form_field_set_value(fields[3], "");
        bp_form_update_fields(form, fields, 4);

        form->status = BP_WINDOW_STATUS_LOOPING;
      }
    }
  } while (form->status == BP_WINDOW_STATUS_LOOPING);

  form->_main_win->status = BP_WINDOW_STATUS_COMPLETE;

  bp_form_destroy(form);

  for (int i = 0; i < 4; i++)
    bp_form_field_destroy(fields[i]);

  free(fields);
}
