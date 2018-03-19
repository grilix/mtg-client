#include "../session.h"
#include "client.h"

  extern ChestResponse *
chest_get_collection(Session *session)
{
  return chest_get(session, "/api/v1/collection", "cards");
}
