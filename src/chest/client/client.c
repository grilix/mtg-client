#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../session.h"
#include "../../bplib/bprequest.h"
#include "../../bplib/bpresponse.h"
#include "client.h"

  extern json_value *
json_object_key(json_value *value, char *key)
{
  if (value == NULL || (value->type != json_object))
    return NULL;

  int length, x;

  length = value->u.object.length;

  for (x = 0; x < length; x++) {
    if (strcmp(value->u.object.values[x].name, key) == 0)
      return value->u.object.values[x].value;
  }

  return NULL;
}

  static json_value *
extract_json(BpResponse *response)
{
  if (response->body == NULL)
    return NULL;

  return json_parse((json_char *)response->body, response->body_len);
}

  static ChestResponse *
chest_response_create(Session *session, BpResponse *response)
{
  ChestResponse *chest_response =
    (ChestResponse *)calloc(1, sizeof(ChestResponse));

  if (chest_response == NULL)
    return NULL;

  chest_response->_response = response;
  chest_response->json = extract_json(response);
  session_extract_header(session, response->headers);

  return chest_response;
}

  static BpUrl *
build_url(char *path)
{
  return bp_url_create("localhost", 3000, path);
}

  static void
destroy_url(BpUrl *url)
{
  free(url->path);
  free(url);
}

  static ChestResponse *
chest_send_request(Session *session, BpRequest *request, char *root_key)
{
  ChestResponse *chest_response = NULL;

  BpResponse *response = bp_send_request(request);

  if (response != NULL)
  {
    chest_response = chest_response_create(session, response);

    if (chest_response == NULL)
      bp_response_destroy(response);
    else
    {
      if (chest_response->json == NULL)
      {
        chest_response->error = strdup("Invalid server response");
        if (chest_response->error == NULL)
        {
          chest_response_destroy(chest_response);
          return NULL;
        }
      }
      else
      {
        json_value *tmp = json_object_key(chest_response->json, "error");

        if (tmp != NULL)
          chest_response->error = strdup(tmp->u.string.ptr);
        else
        {
          tmp = json_object_key(chest_response->json, root_key);

          if (tmp != NULL)
            chest_response->json_root = tmp;
          else
            chest_response->error = strdup("Unexpected server response");
        }
      }
    }
  }

  return chest_response;
}

  extern ChestResponse *
chest_get(Session *session, char *path, char *root_key)
{
  BpUrl *url = build_url(path);

  char **headers = calloc(3, sizeof(char *));
  if (headers == NULL)
  {
    destroy_url(url);
    return NULL;
  }

  headers[0] = "Accept: application/json";
  headers[1] = session_create_header(session);

  ChestResponse *chest_response = NULL;
  BpRequest *request = bp_request_create(url, headers, BP_REQUEST_METHOD_GET);

  if (request != NULL)
  {
    chest_response = chest_send_request(session, request, root_key);

    bp_request_destroy(request);
  }

  if (headers[1] != NULL)
    free(headers[1]);

  bp_url_destroy(url);
  free(headers);

  return chest_response;
}

  extern ChestResponse *
chest_post(Session *session, char *path, char *data, int data_len,
           char *root_key)
{
  BpUrl *url = build_url(path);

  char **headers = calloc(4, sizeof(char *));
  if (headers == NULL)
  {
    bp_url_destroy(url);
    return NULL;
  }

  headers[0] = "Accept: application/json";
  headers[1] = "Content-Type: application/json";
  headers[2] = session_create_header(session);

  ChestResponse *chest_response = NULL;
  BpRequest *request = bp_request_create(url, headers, BP_REQUEST_METHOD_POST);

  if (request != NULL)
  {
    request->body = data;
    request->body_len = data_len;

    chest_response = chest_send_request(session, request, root_key);

    bp_request_destroy(request);
  }

  if (headers[2] != NULL)
    free(headers[2]);

  bp_url_destroy(url);
  free(headers);

  return chest_response;
}

  extern void
chest_response_destroy(ChestResponse *response)
{
  if (response->_response != NULL)
    bp_response_destroy(response->_response);

  if (response->json != NULL)
    json_value_free(response->json);

  if (response->error != NULL)
    free(response->error);

  free(response);
}
