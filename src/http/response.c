#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "response.h"

  extern Response *
response_allocate(int max_data)
{
  Response *response = (Response *)malloc(sizeof(Response));
  response->body = NULL;
  response->body_max = max_data;
  response->body_len = 0;
  return response;
}

  extern void
response_destroy(Response *response)
{
  free(response->body);
  free(response);
}

  extern void
response_read(int sockfd, Response *response)
{
  int received, bytes;

  if (response->body != NULL)
    free(response->body);

  response->body = calloc(response->body_max + 1, sizeof(char));

  received = 0;

  do {
    bytes = read(sockfd,
                 response->body + received,
                 response->body_max - received);

    if (bytes < 0)
    {
      perror("ERROR reading response from socket");
      response->body_len = received;
      return;
    }

    if (bytes == 0)
      break;

    received += bytes;
  } while (received < response->body_max);

  close(sockfd);

  response->body_len = received;

  if (received == response->body_max)
  {
    perror("ERROR storing complete response from socket");
    return;
  }
}

