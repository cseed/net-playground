
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

#include "lib.h"

int
main()
{
  int rc;
  
  int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == -1)
    perror("socket");
  
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6067);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  rc = bind(fd, (struct sockaddr *)&addr, sizeof addr);
  if (rc == -1)
    perror("bind");
  assert(rc == 0);
  
  rc = listen(fd, 5);
  if (rc == -1)
    perror("listen");
  assert(rc == 0);
  
  while (1)
    {
      char buf[1024];
      
      int clientfd = accept(fd, 0, 0);
      if (clientfd == -1)
        perror("accept");
      
      while (1)
        {
          int nread = read(clientfd, buf, sizeof buf);
          if (nread == 0)
            {
              close(clientfd);
              break;
            }
          else if (nread == -1)
            perror("read");
          
          rc = writen(clientfd, buf, nread);
          if (rc == -1)
            perror("write");
        }
    }
}
