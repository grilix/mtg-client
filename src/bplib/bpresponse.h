#ifndef BPLIB_BP_RESPONSE_H
#define BPLIB_BP_RESPONSE_H

struct _BpResponse
{
  char **headers;
  char *body;
  int body_len;
};

typedef struct _BpResponse BpResponse;

  extern BpResponse *
bp_response_allocate(void);

  extern void
bp_response_destroy(BpResponse *response);

  extern void
bp_response_read(int sockfd, BpResponse *response);

  extern char *
bp_response_header_value(BpResponse *response, char *name);

#endif
