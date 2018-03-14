#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../bplib/bpstring.h"
#include "response.h"

  extern Response *
response_allocate(int max_data)
{
  Response *response = (Response *)malloc(sizeof(Response));
  response->headers = NULL;
  response->body = NULL;
  response->max_size = max_data;
  response->body_len = 0;
  return response;
}

  static void
response_destroy_headers(Response *response)
{
  if (response->headers == NULL)
    return;

  char **it = response->headers;
  while (*it)
    free(*(it++));
  free(response->headers);
  response->headers = NULL;
}

  static void
response_reset(Response *response)
{
  response_destroy_headers(response);

  if (response->body != NULL)
  {
    free(response->body);
    response->body = NULL;
  }
}

  extern void
response_destroy(Response *response)
{
  response_reset(response);
  free(response);
}

  static void
response_populate(Response *response, char *data)
{
  char **parts = bpsplit_str(data, "\r\n\r\n", 2);

  response->headers = bpsplit_str(parts[0], "\r\n", 0);
  free(parts[0]);

  /*
   * parts[1] is now owned by response, don't free.
   */
  response->body = parts[1];

  free(parts);
}

  extern void
response_read(int sockfd, Response *response)
{
  int received, bytes;
  char *data;

  response_reset(response);

  data = calloc(response->max_size + 1, sizeof(char));

  received = 0;

  do {
    bytes = read(sockfd, data + received, response->max_size - received);

    if (bytes < 0)
    {
      perror("ERROR reading response from socket");
      free(data);
      return;
    }

    if (bytes == 0)
      break;

    received += bytes;
  } while (received < response->max_size);

  close(sockfd);

  response->body_len = received;

  if (received == response->max_size)
  {
    free(data);
    perror("ERROR storing complete response from socket");
    return;
  }

  response_populate(response, data);

  free(data);
}

