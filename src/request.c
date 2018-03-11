#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "request.h"

#define DEBUG 1

void error(const char *msg)
{
  perror(msg);
  exit(-2);
}

  extern void
send_post(const char *host, int port, const char *path, const char *data,
          int data_len)
{
  char *message_fmt =
    "POST %s HTTP/1.1\r\n" \
    "Host: %s\r\n" \
    "Accept: application/json\r\n" \
    "Content-Type: application/json\r\n" \
    "Content-Length: %d\r\n" \
    "Connection: close\r\n" \
    "\r\n" \
    "%s\r\n";

  struct hostent *server;
  struct sockaddr_in serv_addr;

  int sockfd, bytes, sent, received, total;
  char message[1024], response[4096];

  sprintf(message, message_fmt, path, host, data_len, data);

#if DEBUG
  printf("Request:\n%s\n", message);
#endif

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  server = gethostbyname(host);
  if (server == NULL)
    error("ERROR, no such host");

  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  total = strlen(message);
  sent = 0;
  do {
    bytes = write(sockfd, message + sent, total - sent);
    if (bytes < 0)
      error("ERROR writing message to socket");
    if (bytes == 0)
      break;
    sent += bytes;
  } while (sent < total);

  memset(response, 0, sizeof(response));
  total = sizeof(response) - 1;
  received = 0;
  do {
    bytes = read(sockfd,response+received,total-received);
    if (bytes < 0)
      error("ERROR reading response from socket");
    if (bytes == 0)
      break;
    received += bytes;
  } while (received < total);

  close(sockfd);

  if (received == total)
    error("ERROR storing complete response from socket");

#if DEBUG
  printf("Response:\n%s\n", response);
#endif
}
