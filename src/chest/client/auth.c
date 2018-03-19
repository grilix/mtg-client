#include <stdio.h>

#include "../../bplib/bpresponse.h"
#include "../session.h"
#include "client.h"

  extern ChestResponse *
sign_in(Session *session, char *user, char *password)
{
  char data[1024], len;

  len = sprintf(data, "{\"username\":\"%s\",\"password\":\"%s\"}",
          user, password);

  return chest_post(session, "/api/v1/auth", data, len, "user");
}
