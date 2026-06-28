#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

int main()
{
    int sock_raw;
    unsigned char *buffer = (unsigned char *)malloc(65536);

    struct sockaddr saddr;
    socklen_t saddr_size = sizeof(saddr);

    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    if(sock_raw < 0)
    {
        perror("Socket Error");
        return 1;
    }

    printf("ROLL_NO=241001160007\n");
    printf("ASSIGNED_PROTOCOL=TCP\n\n");

    int packet_count = 0;

    while(packet_count < 20)
    {
        int data_size = recvfrom(
            sock_raw,
            buffer,
            65536,
            0,
            &saddr,
            &saddr_size
        );

        if(data_size < 0)
        {
            perror("recvfrom");
            break;
        }

        struct iphdr *iph = (struct iphdr *)buffer;

        if(iph->protocol != IPPROTO_TCP)
            continue;

        struct tcphdr *tcph =
            (struct tcphdr *)(buffer + iph->ihl * 4);

        struct sockaddr_in source, dest;

        memset(&source, 0, sizeof(source));
        memset(&dest, 0, sizeof(dest));

        source.sin_addr.s_addr = iph->saddr;
        dest.sin_addr.s_addr = iph->daddr;

        packet_count++;

        printf("=====================================\n");
        printf("PACKET_NO=%d\n", packet_count);
        printf("SRC_IP=%s\n", inet_ntoa(source.sin_addr));
        printf("DST_IP=%s\n", inet_ntoa(dest.sin_addr));
        printf("PROTOCOL=TCP\n");
        printf("PROTOCOL_NO=%d\n", iph->protocol);
        printf("TTL=%d\n", iph->ttl);
        printf("PACKET_SIZE=%d\n", data_size);

        // Extra IP Header Field (Task 5)
        printf("IP_VERSION=%d\n", iph->version);

        printf("SRC_PORT=%u\n", ntohs(tcph->source));
        printf("DST_PORT=%u\n", ntohs(tcph->dest));

        printf("TCP_FLAGS=");

        if(tcph->syn) printf("SYN ");
        if(tcph->ack) printf("ACK ");
        if(tcph->fin) printf("FIN ");
        if(tcph->rst) printf("RST ");
        if(tcph->psh) printf("PSH ");
        if(tcph->urg) printf("URG ");

        printf("\n");
    }

    close(sock_raw);
    free(buffer);

    return 0;
}
