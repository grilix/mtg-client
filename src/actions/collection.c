#include <stdlib.h>
#include <ncurses.h>

#include "collection.h"

#include "../http/response.h"
#include "../http/request.h"

  static int
get_collection(void);

  extern void
collection_menu()
{
  refresh();
  def_prog_mode();
  endwin();

  get_collection();
  getch();

  reset_prog_mode();
  refresh();
}
  static int
get_collection(void)
{
  Url url = {.host = "localhost", .port = 3000, .path = "/api/v1/collection"};

  char *headers[] = {
    "Accept: application/json",

    // TODO: Handle the tokens
    "Authorization: Bearer " \
      "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9." \
      "eyJpZCI6MSwidXNlcm5hbWUiOiJncmlsaXgifQ." \
      "OX4cYctGDU2QdSbELcm98N8fp0fe03xTp0l-_cmH_SI",

    "Content-Type: application/json",
    NULL};

  Response *response = response_allocate(1024 * 100);
  Request *request = request_allocate(url, headers);

  send_get(request, response);

  request_destroy(request);
  response_destroy(response);

  return 1;
}

