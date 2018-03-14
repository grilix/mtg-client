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
bp_request_allocate(BpUrl url, char **headers)
{
  BpRequest *request = (BpRequest *)malloc(sizeof(BpRequest));

  request->url = url;
  request->body = NULL;
  request->body_len = 0;
  request->headers = headers;

  return request;
}

  extern void
bp_request_destroy(BpRequest *request)
{
  free(request);
}

  static void
write_request(int sockfd, const char *message, int total)
{
  int sent = 0, bytes;

  do {
    bytes = write(sockfd, message + sent, total - sent);

    if (bytes < 0)
      error("ERROR writing message to socket");

    if (bytes == 0)
      break;

    sent += bytes;
  } while (sent < total);
}

  static int
build_request_body(char *body, int max_len,
                   BpRequest *request, BpRequestMethod method)
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

  switch (method)
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
open_connection(BpUrl url)
{
  struct hostent *server;
  struct sockaddr_in serv_addr;

  int sockfd;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  server = gethostbyname(url.host);
  if (server == NULL)
    error("ERROR, no such host");

  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(url.port);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  return sockfd;
}

  static void
bp_send_request(BpRequest *request, BpRequestMethod method,
                BpResponse *response)
{
  int sockfd;
  int message_len;
  char message[1024];

  message_len = build_request_body(message, 1024, request, method);

  sockfd = open_connection(request->url);

  write_request(sockfd, message, message_len);

  bp_response_read(sockfd, response);
}

  extern void
bp_send_get(BpRequest *request, BpResponse *response)
{
  bp_send_request(request, BP_REQUEST_METHOD_GET, response);
}

  extern void
bp_send_post(BpRequest *request, BpResponse *response)
{
  bp_send_request(request, BP_REQUEST_METHOD_POST, response);
}
