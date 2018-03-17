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

enum _BpRequestStatus
{
  BP_REQUEST_STATUS_CONNECTING,
  BP_REQUEST_STATUS_SENDING,
  BP_REQUEST_STATUS_READING,
  BP_REQUEST_STATUS_FAILED,
  BP_REQUEST_STATUS_SUCCESS,
};

typedef enum _BpRequestStatus BpRequestStatus;

enum _BpRequestError
{
  BP_REQUEST_ERROR_OUT_OF_MEMORY,
  BP_REQUEST_ERROR_CANT_OPEN_SOCKET,
  BP_REQUEST_ERROR_HOST_NOT_FOUND,
  BP_REQUEST_ERROR_CANT_CONNECT,
  BP_REQUEST_ERROR_CANT_WRITE_REQUEST,
};

typedef enum _BpRequestError BpRequestError;

struct _BpRequest
{
  BpUrl url;
  char **headers;
  char *body;
  BpRequestMethod method;
  BpRequestStatus status;
  BpRequestError error;

  char body_len;
};

typedef struct _BpRequest BpRequest;

  extern BpResponse *
bp_send_request(BpRequest *request);

  extern BpRequest *
bp_request_create(BpUrl url, char **headers, BpRequestMethod method);

  extern void
bp_request_destroy(BpRequest *request);

#endif
