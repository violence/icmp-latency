#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define ICMP_ECHO 8


struct icmp_hdr {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short id;
    unsigned short seq;
};


unsigned short checksum(void *buf, int len) {
    unsigned short *data = buf;
    unsigned int sum = 0;
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    if (len == 1) sum += *(unsigned char*) data;

    sum =  (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return ~sum;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ping.exe <IP> [-c <count>] [-i <interval_ms>] [-w <timeout_ms>]\n");
        return 1;
    }

    char *ip     =  argv[1];
    int count    =  1;
    int timeout  =  5000;
    int interval =  1000;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            count = atoi(argv[i + 1]);
            if (count < 1) count = 1;
            i++;

        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            interval = atoi(argv[i + 1]);
            if (interval < 1) interval = 1000;
            i++;

        } else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            timeout = atoi(argv[i + 1]);
            if (timeout < 1) timeout = 1000;
            i++;
        }
    }

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        WSACleanup();
        return 1;
    }

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(ip);

    for (int i = 0; i < count; i++) {
        struct icmp_hdr icmp;
        icmp.type = ICMP_ECHO;
        icmp.code = 0;
        icmp.checksum = 0;
        icmp.id = (unsigned short)GetCurrentProcessId();
        icmp.seq = i + 1;
        icmp.checksum = checksum(&icmp, sizeof(icmp));

        clock_t start = clock();
        int sent = sendto(sock, (char*)&icmp, sizeof(icmp), 0, (struct sockaddr*)&dest, sizeof(dest));
        if (sent <= 0) {
            printf("Failed to send packet\n");
            Sleep(interval);
            continue;
        }

        char buf[1024];
        struct sockaddr_in from;
        int fromlen = sizeof(from);
        int received = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
        clock_t end = clock();

        if (received <= 0) {
            printf("-1\n");
        } else {
            int ms = (int)(((double)(end - start) / CLOCKS_PER_SEC) * 1000.0);
            printf("%d\n", ms);
        }

        Sleep(interval);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
