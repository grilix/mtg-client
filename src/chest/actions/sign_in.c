#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "../../bplib/bpform.h"
#include "../../bplib/bpmessage.h"
#include "../session.h"
#include "../client/auth.h"

#include "sign_in.h"

  extern int
sign_in_form(Session *session)
{
  BpFormField **fields;
  int result;

  fields = (BpFormField **)calloc(4, sizeof(BpFormField *));
  fields[0] = bp_form_field_create(BP_FORM_FIELD_TYPE_LABEL, "Name");
  fields[1] = bp_form_field_create(BP_FORM_FIELD_TYPE_INPUT, "");
  fields[2] = bp_form_field_create(BP_FORM_FIELD_TYPE_LABEL, "Password");
  fields[3] = bp_form_field_create(BP_FORM_FIELD_TYPE_PASSWORD, "");

  BpForm *form = bp_form_create("Sign in", fields, 4, 40, 10, 10, 10);

  do
  {
    result = bp_form_loop(form);

    if (result == BP_FORM_OK)
    {
      bp_form_sync_input(form);

      if (sign_in(session, fields[1]->value, fields[3]->value) == 0)
        result = BP_FORM_OK;
      else
      {
        bp_show_message("Login failed.", 14, 13);
        bp_form_field_set_value(fields[3], "");
        bp_form_update_fields(form, fields, 4);

        result = BP_FORM_ERR;
      }
    }
  } while ((result != BP_FORM_OK) && (result != BP_FORM_CANCEL));

  bp_form_destroy(form);

  for (int i = 0; i < 4; i++)
    bp_form_field_destroy(fields[i]);

  free(fields);

  return result;
}
