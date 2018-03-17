#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "bpstring.h"
#include "bpresponse.h"
#include "bprequest.h"

void
error(const char *msg)
{
  perror(msg);
  exit(-2);
}

  extern BpRequest *
bp_request_create(BpUrl url, char **headers, BpRequestMethod method)
{
  BpRequest *request = (BpRequest *)malloc(sizeof(BpRequest));

  if (request == NULL)
    return NULL;

  request->url = url;
  request->body = NULL;
  request->body_len = 0;
  request->method = method;
  request->headers = headers;

  return request;
}

  extern void
bp_request_destroy(BpRequest *request)
{
  free(request);
}

  static int
write_request(int sockfd, const char *message, int total)
{
  int sent = 0, bytes;

  do {
    bytes = write(sockfd, message + sent, total - sent);

    if (bytes <= 0)
      break;

    sent += bytes;
  } while (sent < total);

  return sent;
}

  static int
request_error(BpRequest *request, BpRequestError error)
{
  request->status = BP_REQUEST_STATUS_FAILED;
  request->error = error;

  return -1;
}

  static int
build_request_body(char *body, int max_len, BpRequest *request)
{
  char *message_fmt =
    "%s %s HTTP/1.1\r\n" \
    "Host: %s\r\n" \
    "%s" \
    "Content-Length: %d\r\n" \
    "Connection: Close\r\n" \
    "\r\n" \
    "%s";

  int message_len;
  char *custom_headers = bp_join_str(request->headers, "\r\n", 1);
  char *method_str;

  if (custom_headers == NULL)
  {
    request_error(request, BP_REQUEST_ERROR_OUT_OF_MEMORY);
    return -1;
  }


  switch (request->method)
  {
    case BP_REQUEST_METHOD_POST:
      method_str = "POST";
      break;
    case BP_REQUEST_METHOD_GET:
      method_str = "GET";
      break;
    default:
      return -1;
      break;
  }

  message_len = snprintf(
      body, max_len, message_fmt,
      method_str, request->url.path, request->url.host, custom_headers,
      request->body_len,
      (request->body == NULL) ? "" : request->body);

  free(custom_headers);
  return message_len;
}

  static int
open_connection(BpRequest *request)
{
  struct hostent *server;
  struct sockaddr_in serv_addr;

  int sockfd;

  request->status = BP_REQUEST_STATUS_CONNECTING;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    return request_error(request, BP_REQUEST_ERROR_CANT_OPEN_SOCKET);

  server = gethostbyname(request->url.host);
  if (server == NULL)
    return request_error(request, BP_REQUEST_ERROR_HOST_NOT_FOUND);

  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(request->url.port);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    return request_error(request, BP_REQUEST_ERROR_CANT_CONNECT);

  return sockfd;
}

  extern BpResponse *
bp_send_request(BpRequest *request)
{
  int sockfd;
  int message_len;
  char message[1024];

  message_len = build_request_body(message, 1024, request);
  if (message_len < 0)
    return NULL;

  sockfd = open_connection(request);
  if (sockfd < 0)
    return NULL;

  request->status = BP_REQUEST_STATUS_SENDING;
  if (write_request(sockfd, message, message_len) < message_len)
  {
    request_error(request, BP_REQUEST_ERROR_CANT_WRITE_REQUEST);
    return NULL;
  }
  request->status = BP_REQUEST_STATUS_READING;

  return bp_response_read(sockfd);
}
