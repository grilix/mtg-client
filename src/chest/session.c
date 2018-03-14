#include <stdio.h> // not needed
#include <string.h>
#include <stdlib.h>

#include "session.h"
#include "../bplib/bprequest.h"
#include "../bplib/bpresponse.h"

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

  extern int
sign_in(Session *session, char *user, char *password)
{
  int result;

  char data[1024], len;
  BpUrl url = {.host = "localhost", .port = 3000, .path = "/api/v1/auth"};

  char *headers[] = {
    "Accept: application/json",
    "Content-Type: application/json",
    NULL};

  BpResponse *response = bp_response_allocate(1024);
  BpRequest *request = bp_request_allocate(url, headers);

  len = sprintf(data, "{\"username\":\"%s\",\"password\":\"%s\"}",
          user, password);

  request->body = data;
  request->body_len = len;

  bp_send_post(request, response);

  result = session_extract_token(session, response->headers);

  bp_request_destroy(request);
  bp_response_destroy(response);

  return result;
}
