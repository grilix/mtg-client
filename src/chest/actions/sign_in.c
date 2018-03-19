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
      ChestResponse *response = sign_in(session,
                                        fields[1]->value, fields[3]->value);

      if (response == NULL)
      {
        bp_window_show_message(form->_main_win, "Cant connect to the server");
        form->status = BP_WINDOW_STATUS_LOOPING;
      }
      else
      {
        if (response->error != NULL)
        {
          bp_window_show_message(form->_main_win, response->error);

          bp_form_field_set_value(fields[3], "");
          bp_form_update_fields(form, fields, 4);

          form->status = BP_WINDOW_STATUS_LOOPING;
        }
        else
          form->status = BP_WINDOW_STATUS_COMPLETE;
        chest_response_destroy(response);
      }
    }

    bp_window_refresh(form->_main_win);
  } while (form->status == BP_WINDOW_STATUS_LOOPING);

  form->_main_win->status = BP_WINDOW_STATUS_COMPLETE;

  bp_form_destroy(form);

  for (int i = 0; i < 4; i++)
    bp_form_field_destroy(fields[i]);

  free(fields);
}
