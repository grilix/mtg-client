#include <stdlib.h>
#include <ncurses.h>

#include "../session.h"
#include "collection.h"

#include "../../bplib/bpresponse.h"
#include "../../bplib/bprequest.h"

  static int
get_collection(Session *session);

  extern void
collection_menu(Session *session)
{
  refresh();
  def_prog_mode();
  endwin();

  get_collection(session);
  getch();

  reset_prog_mode();
  refresh();
}

  static int
get_collection(Session *session)
{
  BpUrl url = {
    .host = "localhost", .port = 3000, .path = "/api/v1/collection"
  };

  char **headers = calloc(4, sizeof(char *));
  headers[0] = "Accept: application/json";
  headers[1] = "Content-Type: application/json";
  headers[2] = session_create_header(session);

  BpResponse *response = bp_response_allocate(1024 * 100);
  BpRequest *request = bp_request_allocate(url, headers);

  bp_send_get(request, response);
  printf("RESULT: %s\n", *response->headers);

  if (headers[2] != NULL)
    free(headers[2]);

  free(headers);
  bp_request_destroy(request);
  bp_response_destroy(response);

  return 1;
}

