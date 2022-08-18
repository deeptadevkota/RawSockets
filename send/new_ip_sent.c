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

struct new_ip
{
    int a;
};

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

    // struct ifreq ifreq_ip;
    // memset(&ifreq_ip, 0, sizeof(ifreq_ip));
    // strncpy(ifreq_ip.ifr_name, "enp0s3", IFNAMSIZ - 1);
    // if (ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0) // getting IP Address
    // {
    //     printf("error in SIOCGIFADDR\n");
    // }

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

    eth->h_proto = htons(0x88b6);
    // printf("%d\n",eth->h_proto);
    total_len += sizeof(struct ethhdr);

    struct new_ip *new_iph = (struct new_ip *)(sendbuff + sizeof(struct ethhdr));
    new_iph->a = 1;
    total_len += sizeof(struct new_ip);

    sendbuff[total_len++] = 0xAA;
    sendbuff[total_len++] = 0xBF;
    sendbuff[total_len++] = 0xCC;
    sendbuff[total_len++] = 0xDD;
    sendbuff[total_len++] = 0xEE;

    // struct sockaddr_ll sadr_ll;
    // sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex;
    // sadr_ll.sll_halen = ETH_ALEN;
    // sadr_ll.sll_addr[0] = DESTMAC0;
    // sadr_ll.sll_addr[1] = DESTMAC1;
    // sadr_ll.sll_addr[2] = DESTMAC2;
    // sadr_ll.sll_addr[3] = DESTMAC3;
    // sadr_ll.sll_addr[4] = DESTMAC4;
    // sadr_ll.sll_addr[5] = DESTMAC5;

    // send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr *)&sadr_ll, sizeof(struct sockaddr_ll));
    // if (send_len < 0)
    // {
    //     printf("error in sending....sendlen = % d....errno = % d\n", send_len, errno);
    //     return -1;
    // }


    char text[] = {"Test message. Test message. Test message!\n"};
    size_t msg_len = strlen(text) + 1; 

    struct iovec iov[1];
    iov[0].iov_base = text;
    iov[0].iov_len = msg_len;

    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex;
    sadr_ll.sll_halen = ETH_ALEN;
    sadr_ll.sll_addr[0] = DESTMAC0;
    sadr_ll.sll_addr[1] = DESTMAC1;
    sadr_ll.sll_addr[2] = DESTMAC2;
    sadr_ll.sll_addr[3] = DESTMAC3;
    sadr_ll.sll_addr[4] = DESTMAC4;
    sadr_ll.sll_addr[5] = DESTMAC5;
    sadr_ll.sll_family = AF_PACKET;

    struct msghdr message;
    message.msg_name = &sadr_ll;
    message.msg_namelen = sizeof(sadr_ll);
    message.msg_iov = iov;
    message.msg_iovlen = 1;
    message.msg_control = 0;
    message.msg_controllen = 0;


    printf("%ld\n",sendmsg(sock_raw, &message, 0));
    // if (sendmsg(sock_raw, &message, 0) == -1)
    // {
    //     printf("Error in sending the message!");
    // }

    printf("Ends here!\n");
}