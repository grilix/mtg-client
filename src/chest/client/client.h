#ifndef CHEST_CLIENT_H
#define CHEST_CLIENT_H

#include "../../bplib/bpresponse.h"

#include "../../json/json.h"

struct _ChestResponse
{
  BpResponse *_response;
  json_value *json;
  json_value *json_root;
  char *error;
};

typedef struct _ChestResponse ChestResponse;

  extern json_value *
json_object_key(json_value *value, char *key);

  extern ChestResponse *
chest_post(Session *session, char *path, char *data, int data_len,
           char *root_key);

  extern ChestResponse *
chest_get(Session *session, char *path, char *root_key);

  extern void
chest_response_destroy(ChestResponse *response);

// Auth
  extern ChestResponse *
sign_in(Session *session, char *user, char *password);

// Collection
  extern ChestResponse *
chest_get_collection(Session *session);

// Decks
  extern ChestResponse *
chest_get_decks(Session *session);

  extern ChestResponse *
chest_get_deck_cards(Session *session, int deck_id);

#endif
