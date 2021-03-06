// raw_sock.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>

struct new_ip
{
    int a;
};

int main()
{
  // Structs that contain source IP addresses
  struct sockaddr_in source_socket_address, dest_socket_address;

  int packet_size;

  // Allocate string buffer to hold incoming packet data
  unsigned char *buffer = (unsigned char *)malloc(65536);
  // Open the raw socket

  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

  if (sock == -1)
  {
    // socket creation failed, may be because of non-root privileges
    perror("Failed to create socket");
    exit(1);
  }
  while (1)
  {
    // recvfrom is used to read data from a socket
    packet_size = recvfrom(sock, buffer, 65536, 0, NULL, NULL);
    if (packet_size == -1)
    {
      printf("Failed to get packets\n");
      return 1;
    }

    struct ethhdr *eth = (struct ethhdr *)(buffer);

    if (eth->h_proto == 46728)
    {
      printf("New-IP packet received!");

      printf("\nEthernet Header\n");
      printf("\t | -Source Address : %.2X - %.2X - %.2X - %.2X - %.2X - %.2X\n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
      printf("\t | -Destination Addresss %.2X - %.2X - %.2X - %.2X - %.2X - %.2X\n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
      printf("\t | -Protocol: %x\n", eth->h_proto);

      struct new_ip *new_iph = (struct new_ip *)(buffer + sizeof(struct ethhdr));
      printf("%d\n",new_iph->a);
    
    }
  }

  return 0;
}