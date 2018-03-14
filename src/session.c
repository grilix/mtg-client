#include <stdio.h> // not needed
#include <string.h>
#include <stdlib.h>

#include "session.h"

  extern Session *
session_allocate(void)
{
  Session *session = (Session *)malloc(sizeof(Session));
  session->token = NULL;
  return session;
}

  extern void
session_destroy(Session *session)
{
  if (session->token != NULL)
    free(session->token);
  free(session);
}

  static void
session_set_token(Session *session, char *token)
{
  if (session->token != NULL)
    free(session->token);

  session->token = strdup(token);
}

  extern int
session_extract_token(Session *session, char **headers)
{
  char **it = headers;
  char *auth_str = "Authorization: Bearer ";
  int auth_len = strlen(auth_str);

  while (*it)
  {
    if (strncmp(*it, auth_str, auth_len) == 0)
      break;
    it++;
  }

  if (!*it)
    return -1;

  session_set_token(session, *(it) + auth_len);

  return 0;
}

  extern char *
session_create_header(Session *session)
{
  if (session->token == NULL)
    return NULL;

  char *auth = "Authorization: Bearer ";
  int total_len = strlen(auth) + strlen(session->token);

  char *header = (char *)calloc(total_len + 1, 1);

  strcat(header, auth);
  strcat(header, session->token);

  return header;
}
