#include "port_configurator.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/if_arp.h>

int set_ip(int fd, struct message* m) {
    struct ifreq ifr;
    struct sockaddr_in *sin;
	char* ifs_name = strtok(m->msg, ";");
	char *ip = strtok(0, ";");
	char *ifs_mask = strtok(0, ";");
	int sock;
	
	if (ip == NULL) {
		return send_message(fd, 1, "You must give ip\n");
	}
	
	if(ifs_mask == NULL) {
		return send_message(fd, 1, "You must give mask\n");
	}
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return send_message(fd, 1, "Error: set ip socket\n");
	}

    //
    // Set the IP address of the new interface/device.
    //
    memset(&ifr,  0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, ifs_name, IFNAMSIZ);

    sin = (struct sockaddr_in*) &ifr.ifr_addr;
    inet_pton(AF_INET, ip, &sin->sin_addr);
    ifr.ifr_addr.sa_family = AF_INET;

    if (ioctl (sock, SIOCSIFADDR, &ifr) < 0) {
        send_message(fd, 1, "Error: wrong interface name or ip\n");
        close(sock);
        return 0;
    }

    //
    // Set the net mask of the new interface/device
    //
    memset(&ifr,  0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, ifs_name, IFNAMSIZ);

    sin = (struct sockaddr_in*) &ifr.ifr_netmask;
    inet_pton(AF_INET, ifs_mask, &sin->sin_addr);
    ifr.ifr_addr.sa_family = AF_INET;

    if (ioctl (sock, SIOCSIFNETMASK, &ifr) < 0) {
        send_message(fd, 1, "Error: wrong mask\n");
        close(sock);
        return 0;
    }

    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
        send_message(fd, 1, "Error: interface is not up\n");
        close(sock);
        return 0;
    }

    close(sock);
    return send_message(fd, 1, "Set ip success\n");
}

int set_mac(int fd, struct message* m) {
    struct ifreq ifr;
    int s;
	char* ifs_name = strtok(m->msg, ";");
	char* mac = strtok(0, ";");
    
	if (mac == NULL) {
		return send_message(fd, 1, "You must give mac\n");
	}

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
        return send_message(fd, 1, "Error: set mac socket\n");
    }
 
    strcpy(ifr.ifr_name, ifs_name);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    if (ioctl(s, SIOCSIFHWADDR, &ifr) < 0) {
        send_message(fd, 1, "Error: wrong interface name\n");
        close(s);
        return 0;
    }
    close(s);
    return send_message(fd, 1, "Set mac success\n");
}

