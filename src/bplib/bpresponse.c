#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bpstring.h"
#include "bpresponse.h"

  extern BpResponse *
bp_response_allocate(int max_data)
{
  BpResponse *response = (BpResponse *)malloc(sizeof(BpResponse));

  response->headers = NULL;
  response->body = NULL;
  response->max_size = max_data;
  response->body_len = 0;

  return response;
}

  static void
bp_response_destroy_headers(BpResponse *response)
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
bp_response_reset(BpResponse *response)
{
  bp_response_destroy_headers(response);

  if (response->body != NULL)
  {
    free(response->body);
    response->body = NULL;
  }
}

  extern void
bp_response_destroy(BpResponse *response)
{
  bp_response_reset(response);
  free(response);
}

  static void
response_populate(BpResponse *response, char *data)
{
  char **parts = bp_split_str(data, "\r\n\r\n", 2);

  response->headers = bp_split_str(parts[0], "\r\n", 0);
  free(parts[0]);

  /*
   * parts[1] is now owned by response, don't free.
   */
  response->body = parts[1];

  free(parts);
}

  extern void
bp_response_read(int sockfd, BpResponse *response)
{
  int received, bytes;
  char *data;

  bp_response_reset(response);

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

