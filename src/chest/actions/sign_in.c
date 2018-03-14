#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "../../bplib/bpform.h"
#include "../../bplib/bpresponse.h"
#include "../../bplib/bprequest.h"

#include "../session.h"
#include "sign_in.h"

  extern int
sign_in_form(Session *session)
{
  BpFormField **fields;

  fields = (BpFormField **)calloc(4, sizeof(BpFormField *));
  fields[0] = bp_form_field_create(BP_FORM_FIELD_TYPE_LABEL, "Name");
  fields[1] = bp_form_field_create(BP_FORM_FIELD_TYPE_INPUT, "");
  fields[2] = bp_form_field_create(BP_FORM_FIELD_TYPE_LABEL, "Password");
  fields[3] = bp_form_field_create(BP_FORM_FIELD_TYPE_PASSWORD, "");

  int result = bp_show_form("Sign in", fields, 4, 40, 10, 10, 10);

  if (result == BP_FORM_OK)
  {
    result = sign_in(session, fields[1]->value, fields[3]->value);

    if (result != 0)
    {
      def_prog_mode();
      endwin();

      printf("Login failed.\n");
      getch();

      reset_prog_mode();
      refresh();
    }
  }

  for (int i = 0; i < 4; i++)
    bp_form_field_destroy(fields[i]);

  free(fields);

  return result;
}
