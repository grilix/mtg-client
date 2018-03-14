#ifndef BPLIB_REQUEST_H
#define BPLIB_REQUEST_H

#include "bpresponse.h"

struct _BpUrl
{
  char *host;
  int port;
  char *path;
};

typedef struct _BpUrl BpUrl;

enum _BpRequestMethod
{
  BP_REQUEST_METHOD_GET,
  BP_REQUEST_METHOD_POST
};

typedef enum _BpRequestMethod BpRequestMethod;

struct _BpRequest
{
  BpUrl url;
  char **headers;
  char *body;
  BpRequestMethod method;
  char body_len;
};

typedef struct _BpRequest BpRequest;

  extern void
bp_send_get(BpRequest *request, BpResponse *response);

  extern void
bp_send_post(BpRequest *request, BpResponse *response);

  extern BpRequest *
bp_request_allocate(BpUrl url, char **headers);

  extern void
bp_request_destroy(BpRequest *request);

#endif
