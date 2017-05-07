
#include <unistd.h>

#include "lib.h"

int
writen(int fd, const void *buf, size_t n)
{
  const void *p = buf;
  int left = n;
  while (left > 0)
    {
      int nwritten = write(fd, p, left);
      if (nwritten == -1)
        return -1;
      left -= nwritten;
      p += nwritten;
    }
  
  return 0;
}

