#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

  extern void
send_post(const char *host, int port, const char *path, const char *data,
          int data_len);

#endif
