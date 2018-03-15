#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bpstring.h"
#include "bpresponse.h"

  extern BpResponse *
bp_response_allocate(void)
{
  BpResponse *response = (BpResponse *)malloc(sizeof(BpResponse));

  response->headers = NULL;
  response->body = NULL;
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

/*
 * Extracts the headers from a raw response and returns the
 * position to the body.
 *
 * Returns -1 if the headers were not found. This case means
 * The headers are not complete, and we must receive another
 * chunk from the server before we can continue.
 *
 * This function will never return 0.
 *
 * Body starting right at the beginning of the string is not
 * allowed and will cause issues.
 */
  static int
bp_response_extract_headers(BpResponse *response, char *data)
{
  char *body = strstr(data, "\r\n\r\n");

  if (!body)
    return -1;

  assert((body - data) > 0);

  char *headers_str = strndup(data, body - data);
  response->headers = bp_split_str(headers_str, "\r\n", 0);

  free(headers_str);

  bp_response_set_body_length(response);

  return (body - data) + 4; // Skip "\r\n\r\n"
}

  extern void
bp_response_read(int sockfd, BpResponse *response)
{
  int received, bytes;

  char *data;

  bp_response_reset(response);

  // Start with 4kb, we might want to rewrite this whole
  // logic to support more content in the headers.
  int max_size = 1024 * 4;

  data = calloc(max_size + 1, sizeof(char));

  received = 0;

  do {
    bytes = read(sockfd, data + received, max_size - received);

    if (bytes < 0)
    {
      perror("ERROR reading response from socket");
      free(data);
      return;
    }

    if (bytes == 0)
      break;

    received += bytes;

    // The first time we should have the headers already
    if (response->headers == NULL)
    {
      int body_start = bp_response_extract_headers(response, data);

      // If this fails, it means we got too much information
      // in the headers.
      assert(body_start > 0);

      // Skip headers on received counter
      received -= body_start;
      max_size = response->body_len;

      /*int missing_content_size = response->body_len - received;*/
      assert(max_size >= received);

      // Initialize a nuw buffer to hold the complete body
      char *tmp = (char *)calloc(response->body_len + 1, sizeof(char));
      // Copy old data
      memcpy(tmp, data + body_start, received);
      // Free old buffer
      free(data);
      // Point old buffer to the new.
      data = tmp;
    }
  } while (received < max_size);

  close(sockfd);

  response->body_len = received;
  assert(received == max_size);

  response->body = data;
}
