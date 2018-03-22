#include <assert.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bpstring.h"
#include "bpresponse.h"

  static void
bp_response_set_body_length(BpResponse *response)
{
  char *header_value = bp_response_header_value(response, "Content-Length");
  int len;

  if (!header_value)
    len = 0;
  else
    len = atoi(header_value);

  response->body_len = len;
}

  static char **
bp_response_extract_headers(BpResponse *response, char *data, int headers_len)
{
  char *headers_str = strndup(data, headers_len);

  if (headers_str == NULL)
    return NULL;

  char **headers = bp_split_str(headers_str, "\r\n", 0);

  free(headers_str);

  return headers;
}

  static BpResponse *
bp_response_create(char *data, int headers_len)
{
  BpResponse *response = (BpResponse *)malloc(sizeof(BpResponse));

  response->headers = bp_response_extract_headers(response, data, headers_len);

  if (response->headers == NULL)
  {
    free(response);
    return NULL;
  }

  bp_response_set_body_length(response);

  response->body = NULL;

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

  extern void
bp_response_destroy(BpResponse *response)
{
  bp_response_destroy_headers(response);

  if (response->body != NULL)
  {
    free(response->body);
    response->body = NULL;
  }
  free(response);
}

  extern char *
bp_response_header_value(BpResponse *response, char *name)
{
  char **it = response->headers;
  int name_len = strlen(name);

  while (*it)
  {
    if (strncasecmp(*it, name, name_len) == 0)
      break;
    it++;
  }

  if (!*it)
    return NULL;

  return *(it) +
         name_len + // Skip header name
         2; // Skip ": "
}

  extern BpResponse *
bp_response_read(int sockfd)
{
  BpResponse *response = NULL;

  int received, bytes;

  char *data;

  // Start with 4kb, we might want to rewrite this whole
  // logic to support more content in the headers.
  int max_size = 1024 * 4;

  data = (char *)malloc((max_size + 1) * sizeof(char));
  if (data == NULL)
    return NULL;

  received = 0;

  do {
    int block_size = max_size - received;

    bytes = read(sockfd, data + received, block_size);

    if (bytes < 0)
    {
      if (response != NULL)
      {
        bp_response_destroy(response);
        response = NULL;
      }
      free(data);
      break;
    }

    // TODO: What does (bytes == 0) even mean?
    assert(bytes != 0);

    received += bytes;

    // The first time we should have the headers already
    if (response == NULL)
    {
      data[received] = '\0';
      char *body = strstr(data, "\r\n\r\n");

      if (body)
      {
        int headers_len = body - data;
        response = bp_response_create(data, headers_len);

        if (response == NULL)
        {
          // No memory for allocating part of the response
          free(data);
          response = NULL;
          break;
        }
        else
        {
          int body_start = headers_len + 4; // Skip "\r\n\r\n"
          // Skip headers on received counter
          received -= body_start;
          max_size = response->body_len;

          assert(max_size >= received);

          // Initialize a nuw buffer to hold the complete body
          char *tmp = (char *)calloc(response->body_len + 1, sizeof(char));

          if (tmp == NULL)
          {
            bp_response_destroy(response);
            free(data);
            response = NULL;
            break;
          }
          else
          {
            // Copy old data
            memcpy(tmp, data + body_start, received);
            // Free old buffer
            free(data);
            // Point old buffer to the new.
            data = tmp;
          }
        }
      }
    }
  } while (received < max_size);

  close(sockfd);

  if (response != NULL)
  {
    response->body_len = received;
    response->body = data;
  }

  return response;
}
