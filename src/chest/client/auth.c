#include <stdio.h>

#include "../../bplib/bpresponse.h"
#include "../session.h"
#include "client.h"
#include "auth.h"

  extern int
sign_in(Session *session, char *user, char *password)
{
  int result;

  char data[1024], len;

  len = sprintf(data, "{\"username\":\"%s\",\"password\":\"%s\"}",
          user, password);

  ChestResponse *response = chest_post(session, "/api/v1/auth", data, len);

  result = session->token == NULL ? 1 : 0;

  chest_response_destroy(response);

  return result;
}
