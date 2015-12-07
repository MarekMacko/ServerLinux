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

	memset(buf, 0, sizeof(buf));	
	family = ifaddr->ifa_addr->sa_family;
	
	
	family = ifaddr->ifa_addr->sa_family;
	memset(buf, 0, sizeof(buf));	

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		if (ifa->ifa_addr->sa_family == family) {
			strcat(buf, ifa->ifa_name);
			strcat(buf, "\t");
		}
	}
	
	freeifaddrs(ifaddr);
	
	return send_message(fd, 1, buf);
}

static int send_if_ipamask(int fd, const char* iface, int family) { 
	
	struct ifaddrs *ifaddr, *ifa;
	char buf[50];
	char addr_buf[INET6_ADDRSTRLEN];
	char mask_buf[20];
	const char* ip, * mask;
	void *addr;
	
	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}

	memset(buf, 0, 50);	
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		
		if (strcmp(ifa->ifa_name, iface) == 0) {
			if (ifa->ifa_addr->sa_family == family) {
				if (family == AF_INET)
					addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				else if (family == AF_INET6) 
					addr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
				
				strcat(buf, iface);
				strcat(buf, "\t");
				// TODO: size inet6
		//		socklen_t size = family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
			 
				ip = inet_ntop(family, addr, addr_buf, INET6_ADDRSTRLEN);
				strcat(buf, ip);
				strcat(buf, "\t");

				addr = &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr;
				mask = inet_ntop(family, addr, mask_buf, INET6_ADDRSTRLEN);
				strcat(buf, mask);
				break;
			}
		}
	}
	
	freeifaddrs(ifaddr);
    return send_message(fd, 1, buf);
}

int send_ifs_status(int fd)
{


	return 1;
}


int send_ifs_info(int fd, struct message *m)
{
	const char if_delim[2] = "-";
	char* msg = m->msg;
	char* end_if_list = strchr(msg, ';');
	char* command = malloc(sizeof(char)*5);
	
	strcpy(command, ++end_if_list);
	char* ifs_list = strtok(msg, ";");
	char* ifs; // single interface name

	ifs = strtok(ifs_list, if_delim);
	while(ifs != 0)
	{	
		if (strcmp(command, "ipv4") == 0) {
			send_if_ipamask(fd, ifs, AF_INET);
		} else if (strcmp(command, "ipv6") == 0) {
			send_if_ipamask(fd, ifs, AF_INET6);
		} else {
			printf("command not recognized\n");
		}
		
		ifs = strtok(NULL, if_delim);
	}
	
	return 1;
}
/*
int send_ifs_addr_mac(int fd)
{
    int t_fd; 
	struct ifreq ifr;
	char buf[NI_MAXHOST];
	char* iface;	
	memset(buf, 0, NI_MAXHOST); 
	t_fd = socket(AF_INET, SOCK_DGRAM, 0); 
							 
	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;
								  
	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
											      
	//get the ip address
	ioctl(fd, SIOCGIFADDR, &ifr);
	
	strcat(buf, iface);
	strcat(buf, "\t");
	//display ip
//	printf("IP address of %s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
	strcat(buf, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr)); 																	      
	strcat(buf, "\t");
	//get the netmask ip
	ioctl(fd, SIOCGIFNETMASK, &ifr);
																				      
	//display netmask
//	printf("Netmask of %s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
	strcat(buf, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr)); 																	      
	   
	printf("ipamask message :%s\n", buf);
	close(t_fd);
    return send_message(fd, 1, buf);
//	return 1;
}*/
