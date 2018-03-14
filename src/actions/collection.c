#include <stdlib.h>
#include <ncurses.h>

#include "../session.h"
#include "collection.h"

#include "../http/response.h"
#include "../http/request.h"

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
  Url url = {.host = "localhost", .port = 3000, .path = "/api/v1/collection"};

  char **headers = calloc(4, sizeof(char *));
  headers[0] = "Accept: application/json";
  headers[1] = "Content-Type: application/json";
  headers[2] = session_create_header(session);

  Response *response = response_allocate(1024 * 100);
  Request *request = request_allocate(url, headers);

  send_get(request, response);
  printf("RESULT: %s\n", *response->headers);

  if (headers[2] != NULL)
    free(headers[2]);

  free(headers);
  request_destroy(request);
  response_destroy(response);

  return 1;
}

