#include <stdlib.h>
#include <ncurses.h>

#include "../session.h"
#include "collection.h"

#include "../client/collection.h"

  extern void
collection_menu(Session *session)
{
  chest_get_collection(session);
}
