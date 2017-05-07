
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <assert.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <net/if.h>

int
parse_nibble(char c)
{
  assert(isxdigit(c));
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else
    {
      assert(c >= 'A' && c <= 'F');
      return c - 'A' + 10;
    }
}

size_t
parse_bytes(const char *src, void *dst)
{
  char *p = dst;
  size_t n = 0;
  while (*src)
    {
      char c1 = *src++;
      char c2 = *src++;
      assert(c2);
      *p++ = (parse_nibble(c1) << 4) | parse_nibble(c2);
      ++n;
    }
  return n;
}

int
main(int argc, const char **argv)
{
  int rc;
  
  if (argc != 4)
    {
      fprintf(stderr, "usage: lldx <if> <eth addr> <payload>\n");
      exit(EXIT_FAILURE);
    }
  
  int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s == -1)
    perror("socket");
  
  struct ifreq req;
  strncpy(req.ifr_name, argv[1], IFNAMSIZ);
  
  rc = ioctl(s, SIOCGIFINDEX, &req);
  if (rc == -1)
    perror("ioctl");
  int ifindex = req.ifr_ifindex;
  
  rc = ioctl(s, SIOCGIFHWADDR, &req);
  if (rc == -1)
    perror("ioctl");
  // hardware address in req.ifr_hwaddr.sa_data
  
  struct ether_addr *dst_eth_addr = ether_aton(argv[2]);
  if (dst_eth_addr == NULL)
    {
      fprintf(stderr, "invalid address: %s\n", argv[2]);
      exit(EXIT_FAILURE);
    }
  
  /* from packet(7):
     
     When you send packets, it is enough to specify sll_family,
     sll_addr, sll_halen, sll_ifindex, and sll_protocol.  The other
     fields should be 0. */
  struct sockaddr_ll dst_addr;
  memset(&dst_addr, 0, sizeof dst_addr);
  dst_addr.sll_family = AF_PACKET;
  dst_addr.sll_protocol = htons(0x0101); // experimental
  dst_addr.sll_ifindex = ifindex;
  dst_addr.sll_hatype = ARPHRD_ETHER;
  dst_addr.sll_halen = 6;
  memcpy(dst_addr.sll_addr, dst_eth_addr, 6);
  
  char buf[1514]; // maximum packet size
  
  size_t k = strlen(argv[3]);
  if (k & 1)
    {
      fprintf(stderr, "payload has odd length\n");
      exit(EXIT_FAILURE);
    }
  size_t n = k >> 1;
  
  if (n > 1500)
    {
      fprintf(stderr, "payload too large: %zd\n", n);
      exit(EXIT_FAILURE);
    }

  // FIXME can the length field be shorter than the minimum payload size?
  size_t packetn = n + 14;
  
  memcpy(buf, dst_eth_addr, 6);
  memcpy(buf + 6, &req.ifr_hwaddr.sa_data, 6);
  
  *(unsigned short *)(buf + 12) = htons(packetn);
  
  parse_bytes(argv[3], buf + 14);
  
  rc = sendto(s, buf, packetn, 0, (struct sockaddr *)&dst_addr, sizeof dst_addr);
  if (rc == -1)
    perror("sendto");
  
  close(s);
  
  return 0;
}
