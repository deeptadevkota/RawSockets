#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

#define DESTMAC0 0xd0
#define DESTMAC1 0x67
#define DESTMAC2 0xe5
#define DESTMAC3 0x12
#define DESTMAC4 0x6f
#define DESTMAC5 0x8f



int main()
{
    int total_len = 0, send_len;
    int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0)
    {
        printf("error in socket\n");
        return -1;
    }
    printf("Socket created!\n");

    struct ifreq ifreq_i;
    memset(&ifreq_i, 0, sizeof(ifreq_i));
    strncpy(ifreq_i.ifr_name, "enp0s3", IFNAMSIZ - 1);
    printf("%s\n", ifreq_i.ifr_name);
    if ((ioctl(sock_raw, SIOCGIFINDEX, &ifreq_i)) < 0) // getting the the Interface index
        printf("error in index ioctl reading");
    else
        printf("index = %d\n", ifreq_i.ifr_ifindex);

    struct ifreq ifreq_c;
    memset(&ifreq_c, 0, sizeof(ifreq_c));
    strncpy(ifreq_c.ifr_name, "enp0s3", IFNAMSIZ - 1);
    if ((ioctl(sock_raw, SIOCGIFHWADDR, &ifreq_c)) < 0) // getting MAC Address
        printf("error in SIOCGIFHWADDR ioctl reading");

    struct ifreq ifreq_ip;
    memset(&ifreq_ip, 0, sizeof(ifreq_ip));
    strncpy(ifreq_ip.ifr_name, "enp0s3", IFNAMSIZ - 1);
    if (ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0) // getting IP Address
    {
        printf("error in SIOCGIFADDR\n");
    }

    unsigned char *sendbuff = (unsigned char *)malloc(64); // increase in case of more data
    memset(sendbuff, 0, 64);
    struct ethhdr *eth = (struct ethhdr *)(sendbuff);
    eth->h_source[0] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]);
    eth->h_source[1] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]);
    eth->h_source[2] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]);
    eth->h_source[3] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]);
    eth->h_source[4] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]);
    eth->h_source[5] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]);

    eth->h_dest[0] = DESTMAC0;
    eth->h_dest[1] = DESTMAC1;
    eth->h_dest[2] = DESTMAC2;
    eth->h_dest[3] = DESTMAC3;
    eth->h_dest[4] = DESTMAC4;
    eth->h_dest[5] = DESTMAC5;

    eth->h_proto = htons(ETH_P_IP);
    total_len += sizeof(struct ethhdr);

   

    struct iphdr *iph = (struct iphdr *)(sendbuff + sizeof(struct ethhdr));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->id = htons(10201);
    iph->ttl = 64;
    iph->protocol = 17;
    iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
    iph->daddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr))); // put destination IP address

    total_len += sizeof(struct iphdr);

    struct udphdr *uh = (struct udphdr *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));
    uh->source = htons(23451);
    uh->dest = htons(23452);
    uh->check = 0;

    total_len += sizeof(struct udphdr);

    sendbuff[total_len++] = 0xAA;
    sendbuff[total_len++] = 0xBF;
    sendbuff[total_len++] = 0xCC;
    sendbuff[total_len++] = 0xDD;
    sendbuff[total_len++] = 0xEE;

    uh->len = htons((total_len - sizeof(struct iphdr) - sizeof(struct ethhdr)));
    iph->tot_len = htons(total_len - sizeof(struct ethhdr));

    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex; // index of interface
    sadr_ll.sll_halen = ETH_ALEN;              // length of destination mac address
    sadr_ll.sll_addr[0] = DESTMAC0;
    sadr_ll.sll_addr[1] = DESTMAC1;
    sadr_ll.sll_addr[2] = DESTMAC2;
    sadr_ll.sll_addr[3] = DESTMAC3;
    sadr_ll.sll_addr[4] = DESTMAC4;
    sadr_ll.sll_addr[5] = DESTMAC5;

    send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr *)&sadr_ll, sizeof(struct sockaddr_ll));
    if (send_len < 0)
    {
        printf("error in sending....sendlen = % d....errno = % d\n", send_len, errno);
        return -1;
    }

    printf("Ends here!\n");
}