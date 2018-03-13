#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

struct _Response
{
  char *body;
  int body_max;
  int body_len;
};

typedef struct _Response Response;

  extern Response *
response_allocate(int max_data);

  extern void
response_destroy(Response *response);

  extern void
response_read(int sockfd, Response *response);

#endif
