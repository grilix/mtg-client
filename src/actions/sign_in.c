#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "../form.h"
#include "sign_in.h"

#include "../request.h"

  static int
sign_in(char *user, char *password);

void sign_in_start()
{
  FormField **fields;

  fields = (FormField **)calloc(4, sizeof(FormField *));
  fields[0] = form_field_create(FORM_FIELD_TYPE_LABEL, "Name");
  fields[1] = form_field_create(FORM_FIELD_TYPE_INPUT, "");
  fields[2] = form_field_create(FORM_FIELD_TYPE_LABEL, "Password");
  fields[3] = form_field_create(FORM_FIELD_TYPE_PASSWORD, "");

  initscr();

  int result = show_form("Sign in", fields, 4, 40, 10, 10, 10);

  if (result)
  {
    def_prog_mode();
    endwin();

    sign_in(fields[1]->value, fields[3]->value);
    getch();

    reset_prog_mode();
    refresh();
  }

  for (int i = 0; i < 4; i++)
    form_field_destroy(fields[i]);

  free(fields);
}

  static int
sign_in(char *user, char *password)
{
  char data[1024], len;
  len = sprintf(data, "{\"username\":\"%s\",\"password\":\"%s\"}",
          user, password);

  send_post("localhost", 3000, "/api/v1/auth", data, len);

  return 1;
}
