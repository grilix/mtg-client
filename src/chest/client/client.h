#ifndef CHEST_CLIENT_H
#define CHEST_CLIENT_H

#include "../../bplib/bpresponse.h"

#include "../../json/json.h"

struct _ChestResponse
{
  BpResponse *_response;
  json_value *json;
};

typedef struct _ChestResponse ChestResponse;

  extern ChestResponse *
chest_post(Session *session, char *path, char *data, int data_len);

  extern ChestResponse *
chest_get(Session *session, char *path);

  extern void
chest_response_destroy(ChestResponse *response);

// Auth
  extern int
sign_in(Session *session, char *user, char *password);

// Collection
  extern ChestResponse *
chest_get_collection(Session *session);

// Decks
  extern ChestResponse *
chest_get_decks(Session *session);

#endif
