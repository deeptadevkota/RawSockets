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

int main(void)
{
    unsigned char dest[ETH_ALEN] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11}; // destination MAC address

    int s;
    unsigned short protocol = htons(ETH_P_ALL);

    // Message variables
    char message[] = {"Test message. Test message. Test message!\n"};
    size_t msg_len = strlen(message) + 1; // Message length includes null terminator
    int e;                                // Error code
    struct msghdr msg;
    struct iovec msgvec;

    // Setup source-side socket
    s = socket(AF_PACKET, SOCK_RAW, protocol);
    if (s < 0)
    {
        printf("%d: %s\n", errno, strerror(errno));
        return 0;
    }

    msgvec.iov_base = message;
    msgvec.iov_len = msg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = dest;
    msg.msg_namelen = ETH_ALEN;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;
    msg.msg_iov = &msgvec;
    msg.msg_iovlen = 1;

    for (int i = 0; i < 10; i++)
    {
        e = sendmsg(s, &msg, 0);
    }
    printf("message is sent!\n");
    return 0;
}