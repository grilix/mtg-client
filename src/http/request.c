#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "response.h"
#include "request.h"

#define DEBUG 1

void
error(const char *msg)
{
  perror(msg);
  exit(-2);
}

  extern Request *
request_allocate(Url url, char **headers)
{
  Request *request = (Request *)malloc(sizeof(Request));
  request->url = url;
  request->body = NULL;
  request->body_len = 0;
  request->headers = headers;
  return request;
}

  extern void
request_destroy(Request *request)
{
  free(request);
}

  static void
write_request(int sockfd, const char *message, int total)
{
  int sent, bytes;

  sent = 0;
  do {
    bytes = write(sockfd,
                  message + sent,
                  total - sent);

    if (bytes < 0)
      error("ERROR writing message to socket");

    if (bytes == 0)
      break;

    sent += bytes;
  } while (sent < total);
}

  static int
build_request_body(char *body, int max_len,
                   Request *request, RequestMethod method)
{
  char *message_fmt =
    "%s %s HTTP/1.1\r\n" \
    "Host: %s\r\n" \
    "%s" \
    "Content-Length: %d\r\n" \
    "Connection: close\r\n" \
    "\r\n" \
    "%s";

  int message_len;

  // FIXME: sizeof(request->headers) * something?
  char *headers = (char *)malloc(1024 + 1);
  int header_len = 0;
  char **header = request->headers;
  char *method_str;

  switch (method)
  {
    case REQUEST_METHOD_POST:
      method_str = "POST";
      break;
    case REQUEST_METHOD_GET:
      method_str = "GET";
      break;
    default:
      return -1;
      break;
  }

  while (*header && (header_len < 1024))
  {
    strncat(headers, *header, 1024 - header_len);
    header_len += strlen(*header);

    strncat(headers, "\r\n", 1024 - header_len);
    header_len += 2;
    header++;
  }

  // TODO: does this include null char?
  if (header_len == 1024)
  {
    free(headers);
    return -1;
  }

  message_len = snprintf(
      body, max_len, message_fmt,
      method_str, request->url.path, request->url.host, headers,
      request->body_len,
      (request->body == NULL) ? "" : request->body);

  free(headers);
  return message_len;
}

  static int
open_connection(Url url)
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
send_request(Request *request, RequestMethod method, Response *response)
{
  int sockfd;
  int message_len;
  char message[1024];

  sockfd = open_connection(request->url);

  message_len = build_request_body(message, 1024, request, method);

#if DEBUG
  printf("Request:\n%s\n", message);
#endif

  write_request(sockfd, message, message_len);

  response_read(sockfd, response);

#if DEBUG
  printf("Response:\n%s\n", response->body);
#endif
}

  extern void
send_get(Request *request, Response *response)
{
  send_request(request, REQUEST_METHOD_GET, response);
}

  extern void
send_post(Request *request, Response *response)
{
  send_request(request, REQUEST_METHOD_POST, response);
}
