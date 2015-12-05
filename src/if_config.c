#include "if_config.h"
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
//debug
#include <stdio.h>

#define MAXIFS 10 // do zmiany 
#define IF_LEN_NAME 10 // do zmiany

int send_ifs_names(int fd)
{
	struct ifaddrs *ifaddr, *ifa;
	char buf[MAXIFS*IF_LEN_NAME];
	int family;
	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}
	
	family = ifaddr->ifa_addr->sa_family;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		printf("family %d\n", family);
		if (ifa->ifa_addr->sa_family == family);
			printf("name: %s\n", ifa->ifa_name);

		strcat(buf, ifa->ifa_name);
		strcat(buf, " ");
	}
	
	freeifaddrs(ifaddr);
	printf("%s",buf);
	return send_message(fd, 1, buf);
}

int send_ifs_status(int fd)
{


	return 1;
}


int send_ifs_info(int fd, struct message *m)
{
	return 1;
}

int send_ifs_addr_mac(int fd)
{
	struct ifaddrs *ifaddr, *ifa;
	char host[NI_MAXHOST];
	int family, s;
	socklen_t addr_len = sizeof(struct sockaddr_in);

	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}
		
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		family = ifa->ifa_addr->sa_family;
		if (ifa->ifa_addr->sa_family == AF_INET) {
			s = getnameinfo(ifa->ifa_addr, addr_len, host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			
			if (s) {			
				perror("getnameinfo");
				exit(1);
			}
			get_if_ipamask(ifa->ifa_name, family);
		}
	}
	freeifaddrs(ifaddr);

	return 1;
}

static int get_if_ipamask(const char* interface, int family) { 
    int fd; 
	struct ifreq ifr;
	char buff[NI_MAXHOST];
	printf("maxhost :%d\n",NI_MAXHOST);
	const char *iface = interface;
	//iface=interface;
					    
	fd = socket(family, SOCK_DGRAM, 0); 
							 
	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = family;
								  
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
