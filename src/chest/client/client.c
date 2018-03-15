#include <stdio.h>
#include <stdlib.h>

#include "../session.h"
#include "../../bplib/bprequest.h"
#include "../../bplib/bpresponse.h"
#include "client.h"

  static void
extract_json(ChestResponse *response)
{
  if (response->_response->body == NULL)
  {
    response->json = NULL;
    return;
  }

  response->json = json_parse(
      (json_char *)response->_response->body,
      response->_response->body_len);
}

// FIXME: Duplicated code
  extern ChestResponse *
chest_get(Session *session, char *path)
{
  BpUrl url = {.host = "localhost", .port = 3000, .path = path};

  char **headers = calloc(3, sizeof(char *));

  headers[0] = "Accept: application/json";
  headers[1] = session_create_header(session);

  BpRequest *request = bp_request_allocate(url, headers);

  ChestResponse *response = (ChestResponse *)malloc(sizeof(ChestResponse));
  response->_response = bp_response_allocate();

  bp_send_get(request, response->_response);
  extract_json(response);

  session_extract_header(session, response->_response->headers);

  bp_request_destroy(request);

  if (headers[1] != NULL)
    free(headers[1]);

  free(headers);

  return response;
}

  extern ChestResponse *
chest_post(Session *session, char *path, char *data, int data_len)
{
  BpUrl url = {.host = "localhost", .port = 3000, .path = path};

  char **headers = calloc(4, sizeof(char *));

  headers[0] = "Accept: application/json";
  headers[1] = "Content-Type: application/json";
  headers[2] = session_create_header(session);

  BpRequest *request = bp_request_allocate(url, headers);

  ChestResponse *response = (ChestResponse *)malloc(sizeof(ChestResponse));
  response->_response = bp_response_allocate();

  request->body = data;
  request->body_len = data_len;

  bp_send_post(request, response->_response);
  extract_json(response);

  session_extract_header(session, response->_response->headers);

  bp_request_destroy(request);

  if (headers[2] != NULL)
    free(headers[2]);

  free(headers);

  return response;
}

  extern void
chest_response_destroy(ChestResponse *response)
{
  bp_response_destroy(response->_response);

  if (response->json != NULL)
    json_value_free(response->json);

  free(response);
}
