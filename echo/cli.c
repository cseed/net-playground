
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#include "lib.h"

int
main(int argc, const char **argv)
{
  int rc;

  if (argc != 3)
    {
      fprintf(stderr, "usage: cli <server addr> <msg>\n");
      exit(EXIT_FAILURE);
    }
  
  int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1)
    perror("socket");
  
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6067);
  
  in_addr_t s_addr = inet_addr(argv[1]);
  if (s_addr == INADDR_NONE)
    {
      fprintf(stderr, "inet_addr: invalid address: %s\n", argv[1]);
      exit(EXIT_FAILURE);
    }
  addr.sin_addr.s_addr = s_addr;
  
  rc = connect(fd, (struct sockaddr *)&addr, sizeof addr);
  if (rc == -1)
    perror("connect");
  
  rc = writen(fd, argv[2], strlen(argv[2]));
  if (rc == -1)
    perror("write");
  
  char newline = '\n';
  rc = writen(fd, &newline, 1);
  if (rc == -1)
    perror("write");
  
  rc = shutdown(fd, SHUT_WR);
  if (rc == -1)
    perror("shutdown");
  
  while (1)
    {
      char buf[1024];
      int nread = read(fd, buf, sizeof buf);
      if (nread == 0)
        {
          close(fd);
          break;
        }
      else if (nread == -1)
        perror("read");
      
      rc = writen(1, buf, nread);
      if (rc == -1)
        perror("write");
    }
  
  return 0;
}
