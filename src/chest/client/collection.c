#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <ncurses.h>

#include "../../bplib/bpmessage.h"
#include "../session.h"
#include "client.h"
#include "collection.h"

#include "../../json/json.h"

  extern void
chest_get_collection(Session *session)
{
  ChestResponse *response = chest_get(session, "/api/v1/collection");

  bp_show_plain_message("Response is: <%s>\n", response->_response->body);

  if (response->json == NULL)
    bp_show_message("Can't process the response.", 10, 10);
  else
  {
    if (strcmp(response->json->u.object.values[0].name, "error") == 0)
    {
      bp_show_message(
          response->json->u.object.values[0].value->u.string.ptr, 10, 10);
    }
    else
      bp_show_message("Done!", 10, 10);
  }

  chest_response_destroy(response);
}
