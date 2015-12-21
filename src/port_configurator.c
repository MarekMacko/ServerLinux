#include "port_configurator.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/if_arp.h>

int set_ip(const char *deviceName, const char *ip, const char *netmask){
    struct ifreq ifr;
    struct sockaddr_in *sin;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("set ip socket");
        return -1;
    }

    //
    // Set the IP address of the new interface/device.
    //
    memset(&ifr,  0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, deviceName, IFNAMSIZ);

    sin = (struct sockaddr_in*) &ifr.ifr_addr;
    inet_pton(AF_INET, ip, &sin->sin_addr);
    ifr.ifr_addr.sa_family = AF_INET;

    if(ioctl (sock, SIOCSIFADDR, &ifr) < 0) {
        perror("set ip ioctl");
        close(sock);
        return -1;
    }

    //
    // Set the net mask of the new interface/device
    //
    memset(&ifr,  0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, deviceName, IFNAMSIZ);

    sin = (struct sockaddr_in*) &ifr.ifr_netmask;
    inet_pton(AF_INET, netmask, &sin->sin_addr);
    ifr.ifr_addr.sa_family = AF_INET;

    if (ioctl (sock, SIOCSIFNETMASK, &ifr) < 0) {
        perror("Error mask: ");
        close(sock);
        return -1;
    }

    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
        perror("Error if up: ");
        close(sock);
        return 1;
    }

    close(sock);

    return 0;
}

int set_mac(const char *deviceName, const char mac[]){
    struct ifreq ifr;
    int s;
    
    sscanf(mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
        &ifr.ifr_hwaddr.sa_data[0],
        &ifr.ifr_hwaddr.sa_data[1],
        &ifr.ifr_hwaddr.sa_data[2],
        &ifr.ifr_hwaddr.sa_data[3],
        &ifr.ifr_hwaddr.sa_data[4],
        &ifr.ifr_hwaddr.sa_data[5]
        );
    //printf("%s\n", &ifr.ifr_hwaddr.sa_data[0]);
 
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("set mac socket");
        return 1;
    }
 
    strcpy(ifr.ifr_name, deviceName);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    if(ioctl(s, SIOCSIFHWADDR, &ifr)<0){
        perror("set mac ioctl");
        close(s);
        return -1;
    }
    close(s);
    return 0;
}

