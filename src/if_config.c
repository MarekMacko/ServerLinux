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
	//int family=0;
	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}
	
	//family = ifaddr->ifa_addr->sa_family;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
//		printf("family %d\n", family);
		if (ifa->ifa_addr->sa_family == AF_INET);
			printf("name: %s\n", ifa->ifa_name);

		strcat(buf, ifa->ifa_name);
		strcat(buf, " ");
	}
	
	freeifaddrs(ifaddr);
	printf("%s",buf);
	return send_message(fd, 1, buf);
}

int send_if_ipamask(int fd, const char* interface, int family) { 
    int t_fd; 
	struct ifreq ifr;
	char buf[NI_MAXHOST];
	
	const char *iface = interface;
	//iface=interface;
	memset(buf, 0, NI_MAXHOST); 
	t_fd = socket(family, SOCK_DGRAM, 0); 
							 
	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = family;
								  
	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
											      
	//get the ip address
	ioctl(fd, SIOCGIFADDR, &ifr);
	
	strcat(buf, interface);
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
//		printf("ifs = %s\n", ifs);
		
		if (strcmp(command, "ipv4") == 0) {
			send_if_ipamask(fd, ifs, AF_INET);
//			printf("ip4 \n");
		} else if (strcmp(command, "ipv6") == 0) {
//			printf("ip6 \n");
			send_if_ipamask(fd, ifs, AF_INET6);
		} else {
			printf("command not recognized\n");
		}
		
		ifs = strtok(NULL, if_delim);
	}
	
	return 1;
}

int send_ifs_addr_mac(int fd)
{
/*	struct ifaddrs *ifaddr, *ifa;
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
*/
	return 1;
}

