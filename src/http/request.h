#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include "response.h"

struct _Url
{
  char *host;
  int port;
  char *path;
};

typedef struct _Url Url;

enum _RequestMethod
{
  REQUEST_METHOD_GET,
  REQUEST_METHOD_POST
};

typedef enum _RequestMethod RequestMethod;

struct _Request
{
  Url url;
  char **headers;
  char *body;
  RequestMethod method;
  char body_len;
};

typedef struct _Request Request;

  extern void
send_get(Request *request, Response *response);

  extern void
send_post(Request *request, Response *response);

  extern Request *
request_allocate(Url url, char **headers);

  extern void
request_destroy(Request *request);

#endif
