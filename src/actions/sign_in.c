#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "../session.h"
#include "../form.h"
#include "sign_in.h"

#include "../http/response.h"
#include "../http/request.h"

  static int
sign_in(Session *session, char *user, char *password);

  extern int
sign_in_form(Session *session)
{
  FormField **fields;

  fields = (FormField **)calloc(4, sizeof(FormField *));
  fields[0] = form_field_create(FORM_FIELD_TYPE_LABEL, "Name");
  fields[1] = form_field_create(FORM_FIELD_TYPE_INPUT, "");
  fields[2] = form_field_create(FORM_FIELD_TYPE_LABEL, "Password");
  fields[3] = form_field_create(FORM_FIELD_TYPE_PASSWORD, "");

  int result = show_form("Sign in", fields, 4, 40, 10, 10, 10);

  if (result == FORM_OK)
  {
    result = sign_in(session, fields[1]->value, fields[3]->value);

    if (result == FORM_ERR)
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
    form_field_destroy(fields[i]);

  free(fields);

  return result;
}

  static int
sign_in(Session *session, char *user, char *password)
{
  int result;

  char data[1024], len;
  Url url = {.host = "localhost", .port = 3000, .path = "/api/v1/auth"};

  char *headers[] = {
    "Accept: application/json",
    "Content-Type: application/json",
    NULL};

  Response *response = response_allocate(1024);
  Request *request = request_allocate(url, headers);

  len = sprintf(data, "{\"username\":\"%s\",\"password\":\"%s\"}",
          user, password);

  request->body = data;
  request->body_len = len;

  send_post(request, response);

  result = session_extract_token(session, response->headers);

  request_destroy(request);
  response_destroy(response);

  if (result == 0)
    return FORM_OK;

  return FORM_ERR;
}
