
#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void
print_bytes(void *p, size_t n)
{
  for (size_t i = 0; i < n; ++i)
    printf("%02x", *(unsigned char *)(p + i));
}

void
print_family(int domain)
{
  printf("%d", domain);
  switch (domain)
    {
    case AF_UNIX: // and AF_LOCAL
      printf(" unix");
      break;

    case AF_INET:
      printf(" inet");
      break;
      
    case AF_INET6:
      printf(" inet6");
      break;
      
    case AF_PACKET:
      printf(" packet");
      break;
    }
}

void
print_eth_protocol(int protocol)
{
  printf("%04x", protocol);

  switch (protocol)
    {
    case ETH_P_LOOP:
      printf(" loop");
      break;
      
    case ETH_P_IP:
      printf(" ip");
      break;
      
    case ETH_P_802_2:
      printf(" 802.2 frame");
      break;
    }
}

void
print_arp_type(int hatype)
{
  printf("%d", hatype);
  switch (hatype)
    {
    case ARPHRD_ETHER:
      printf(" ether");
    }
}

void
print_packet_type(int pkttype)
{
  printf("%d", pkttype);

  switch (pkttype)
    {
    case PACKET_HOST:
      printf(" host");
      break;

    case PACKET_BROADCAST:
      printf(" broadcast");
      break;

    case PACKET_MULTICAST:
      printf(" multicast");
      break;

    case PACKET_OTHERHOST:
      printf(" otherhost");
      break;

    case PACKET_OUTGOING:
      printf(" outgoing");
      break;
    }
}

int
main()
{
  int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s == -1)
    perror("socket");
  
  while (1)
    {
      char buf[8192];
      struct sockaddr_ll src_addr;
      socklen_t src_addr_len = sizeof src_addr;
      ssize_t n = recvfrom(s, buf, sizeof buf, 0, (struct sockaddr *)&src_addr, &src_addr_len);
      if (n == -1)
	perror("recvfrom");
      
      printf("packet %ld\n", n);
      
      printf("  src_addr\n");
      printf("    family ");
      print_family(src_addr.sll_family);
      printf("\n");
      
      printf("    protocol ");
      print_eth_protocol(ntohs(src_addr.sll_protocol));
      printf("\n");
      
      printf("    ifindex %d\n", src_addr.sll_ifindex);
      printf("    hatype ");
      print_arp_type(src_addr.sll_hatype);
      printf("\n");
      
      printf("    pkttype ");
      print_packet_type(src_addr.sll_pkttype);
      printf("\n");
      
      printf("    halen %d\n", src_addr.sll_halen);
      printf("    addr ");
      print_bytes(src_addr.sll_addr, src_addr.sll_halen);
      printf("\n");
      
      if (src_addr.sll_hatype == ARPHRD_ETHER
	  && n >= 14)
	{
	  printf("  eth header\n");
	  printf("    dest addr ");
	  print_bytes(buf, 6);
	  printf("\n");
	  
	  printf("    src addr ");
	  print_bytes(buf + 6, 6);
	  printf("\n");
	  
	  int ethertype = ntohs(*(unsigned short *)(buf + 12));
	  if (ethertype <= 1500)
	    printf("    size %d\n", ethertype);
	  else 
	    printf("    ether type %04x\n", ethertype);
	  
	  printf ("    payload ");
	  print_bytes(buf + 14, n - 14);
	  printf("\n");
	}
      else
	{
	  printf("  data ");
	  print_bytes(buf, n);
	  printf("\n");
	}
    }
  
  close(s);
  
  return 0;
}
