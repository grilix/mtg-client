#ifndef BPLIB_BP_RESPONSE_H
#define BPLIB_BP_RESPONSE_H

struct _BpResponse
{
  char **headers;
  char *body;
  int max_size;
  int body_len;
};

typedef struct _BpResponse BpResponse;

  extern BpResponse *
bp_response_allocate(int max_data);

  extern void
bp_response_destroy(BpResponse *response);

  extern void
bp_response_read(int sockfd, BpResponse *response);

#endif
