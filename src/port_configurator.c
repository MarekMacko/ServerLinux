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
    if(sock<0){
        perror("Blad podczas tworzenia socketu");
        return 1;
    }

    //
    // Set the IP address of the new interface/device.
    //
    memset(&ifr,  0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, deviceName, IFNAMSIZ);

    sin = (struct sockaddr_in*) &ifr.ifr_addr;
    inet_pton(AF_INET, ip, &sin->sin_addr);
    ifr.ifr_addr.sa_family = AF_INET;

    if(ioctl (sock, SIOCSIFADDR, &ifr)<0){
        perror("Error ip: ");
        return 1;
    }

    //
    // Set the net mask of the new interface/device
    //
    memset(&ifr,  0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, deviceName, IFNAMSIZ);

    sin = (struct sockaddr_in*) &ifr.ifr_netmask;
    inet_pton(AF_INET, netmask, &sin->sin_addr);
    ifr.ifr_addr.sa_family = AF_INET;

    if (ioctl (sock, SIOCSIFNETMASK, &ifr)<0)
    {
        perror("Error mask: ");
        return 1;
    }

    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0){
        perror("Error if up: ");
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
    printf("%s\n", &ifr.ifr_hwaddr.sa_data[0]);
 
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s<0){
        perror("Blad podczas tworzenia socketu");
        return 1;
    }
 
    strcpy(ifr.ifr_name, deviceName);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    if(ioctl(s, SIOCSIFHWADDR, &ifr)<0){
        perror("Error set mac: ");
        return 1;
    }
 
    return 0;
}

int get_if_info(const char* interface){//jak Ci się przyda to bierz, jak nie to usuwam :D
	int fd;
    struct ifreq ifr;
     
    const char *iface = interface;
    iface=interface;
     
    fd = socket(AF_INET, SOCK_DGRAM, 0);
 
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
 
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
     
    //get the ip address
    ioctl(fd, SIOCGIFADDR, &ifr);
     
    //display ip
    printf("IP address of %s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
     
    //get the netmask ip
    ioctl(fd, SIOCGIFNETMASK, &ifr);
     
    //display netmask
    printf("Netmask of %s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
     
    close(fd);

	return 0;
}