#include "../session.h"
#include "client.h"

  extern ChestResponse *
chest_get_decks(Session *session)
{
  return chest_get(session, "/api/v1/decks");
}
