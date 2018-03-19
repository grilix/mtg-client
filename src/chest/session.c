#include <stdio.h>
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
session_extract_header(Session *session, char **headers)
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
  int auth_len = strlen(auth);
  int token_len = strlen(session->token);
  int total_len = auth_len + token_len;

  char *header = (char *)calloc(total_len + 1, 1);

  memcpy(header, auth, auth_len);
  memcpy(header + auth_len, session->token, token_len);

  return header;
}

  extern void
session_load(Session *session, const char *filename)
{
  char token[255];
  FILE *file;

  // There's no real min/max length for the token,
  // this is just a guess.
  int min_len = 2 + // dots for: header.value.signature
                4 + // minimun header: "{}" = "e30K"
                4 + // minimum body: "{}" = "e30K"
                1;  // signature could be anything

  if ((file = fopen(".token", "r")) == NULL)
    return;

  if (fgets(token, 255, file) == NULL)
    return;

  if (strlen(token) > min_len)
    session_set_token(session, token);
}

  extern void
session_save(Session *session, const char *filename)
{
  FILE *file;

  if ((file = fopen(".token", "w")) == NULL)
    return;

  if (session->token != NULL)
    fputs(session->token, file);
  else
    fputs("", file);
}

  extern void
session_drop(Session *session)
{
  if (session->token == NULL)
    return;

  free(session->token);
  session->token = NULL;
}
