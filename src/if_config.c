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
#include <stdio.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

#define MAXIFS 10
#define IF_LEN_NAME 10
#define MAX_LEN_COM 7
#define MAC_ADDR_LEN 18
#define STATUS_LEN 5

static char * get_ifs_all_names(void)
{
	struct ifaddrs *ifaddr, *ifa;
	char *ifs_all_names = malloc(sizeof(char)*MAXIFS*IF_LEN_NAME);
	int family;

	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}

	family = ifaddr->ifa_addr->sa_family;
	memset(ifs_all_names, 0, MAXIFS*IF_LEN_NAME);	
		
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == family) {
			if (ifs_all_names[0] != '\0') {
				strcat(ifs_all_names, "-");
			}
			strcat(ifs_all_names, ifa->ifa_name);
		}
	}
	freeifaddrs(ifaddr);
	return ifs_all_names;
}

int send_ifs_all_names(int fd)
{
	char *all_names = get_ifs_all_names();
	int len = strlen(all_names);
	int i;
	for (i = 0; i < len; i++) {
		if (all_names[i] == '-') {
			all_names[i] = '\n';
		}
	}
	return send_message(fd, 1, all_names);
}

static char * get_client_ifs_names(char *msg)
{
	char *end_ifs_list = NULL;
	const int names_buf_size = MAXIFS*IF_LEN_NAME;
	char *ifs_names = malloc(sizeof(char)*names_buf_size);

	if (msg == NULL || !strcmp(msg, "")) {
		return NULL;
	}

	end_ifs_list = strchr(msg, ';');	
	if (end_ifs_list == NULL) {
		return NULL;
	}
	
	memset(ifs_names, 0, names_buf_size);
	int i = 0;
	while (msg[i] != *end_ifs_list) {
		ifs_names[i] = msg[i];
		i++;
		if (i >= names_buf_size) {
			break;
		}
	}
	
	if (strcmp(ifs_names, "all") == 0) {
		return get_ifs_all_names();
	}

	return ifs_names;
}

static char * get_client_command(char *msg)
{
	char *command = NULL;
	char *end_ifs_list = NULL;
	
	if (msg == NULL || !strcmp(msg, "")) {
		return NULL;
	}
	
	end_ifs_list = strchr(msg, ';');	
	if (end_ifs_list == NULL) {
		return NULL;
	}
	
	command = ++end_ifs_list;
	if (strcmp(command, "ipv4") == 0) {
		return command;
	} else if (strcmp(command, "ipv6") == 0) {
		return command;
	} else if (strcmp(command, "status") == 0) {
		return command;
	} else if (strcmp(command, "mac") == 0) {
		return command;
	} 
	
	return NULL;
}

static int send_ifs_mac(int fd, char *ifs_name) 
{
	struct ifaddrs *ifaddr, *ifa;
	char mac_buf[MAC_ADDR_LEN];
	char out_buf[IF_LEN_NAME + MAC_ADDR_LEN];

	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}
	
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		
		if (strcmp(ifa->ifa_name, ifs_name) == 0) {	
			if (ifa->ifa_addr->sa_family == AF_PACKET) {
				memset(mac_buf, 0, sizeof(mac_buf));
				strcat(out_buf, ifs_name);
				strcat(out_buf, "\t");
				struct sockaddr_ll* m_addr = (struct sockaddr_ll*)ifa->ifa_addr;
				int i;
				int len = 0;
				for (i = 0; i < 6; i++) {
					len += sprintf(mac_buf+len, "%02X%s", m_addr->sll_addr[i], i < 5 ? ":" : "");
				}
			
				strcat(out_buf, mac_buf);
				strcat(out_buf, "\n");
				freeifaddrs(ifaddr);	
				return send_message(fd, 1, out_buf);
			}
		}
	}

	freeifaddrs(ifaddr);	
	sprintf(out_buf, "Interface %s not found\n", ifs_name);
	return send_message(fd, 1, out_buf);
}

static int send_ifs_status(int fd, char *ifs_name)
{
	struct ifaddrs *ifaddr, *ifa;
	char out_buf[IF_LEN_NAME + STATUS_LEN];
	int family;

	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}
	
	family = ifaddr->ifa_addr->sa_family;
	
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		if (ifa->ifa_addr->sa_family == family) {
			memset(out_buf, 0, sizeof(out_buf));
			strcat(out_buf, ifs_name);
			strcat(out_buf, "\t");
			if (ifa->ifa_flags & IFF_RUNNING) {
				strcat(out_buf, "UP\n");
			} else {
				strcat(out_buf, "DOWN\n");
			}
			return send_message(fd, 1, out_buf);
		}
	}
	freeifaddrs(ifaddr);
	sprintf(out_buf, "Interface %s not found\n", ifs_name);
	return send_message(fd, 1, out_buf);
}

static int send_ifs_ip(int fd, char *ifs_name, int family)
{	
	struct ifaddrs *ifaddr, *ifa;
	socklen_t addr_len = (family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;//16 or 46
	char out_buf[IF_LEN_NAME + addr_len];
	char addr_buf[addr_len];
	char mask_buf[addr_len];
	void *addr;

	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}
	
	memset(out_buf, 0, sizeof(out_buf));
	memset(addr_buf, 0, sizeof(addr_buf));
	memset(mask_buf, 0, sizeof(mask_buf));
	
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
				
		if (strcmp(ifa->ifa_name, ifs_name) == 0) {	
			if (ifa->ifa_addr->sa_family == family) {
				if (family == AF_INET) {
					addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				} else if (family == AF_INET6) {
					addr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
				}

				strcat(out_buf, ifs_name);
				strcat(out_buf, "\t");
				 
				if (inet_ntop(family, addr, addr_buf, addr_len) == NULL) {
    				return send_message(fd, 1, "Translate address error\n");
				}
		
				strcat(out_buf, addr_buf);
				strcat(out_buf, "\t");
				
				if (family == AF_INET) {
					addr = &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr;
				} else if (family == AF_INET6) {
					addr = &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr;
				}
				
				if (inet_ntop(family, addr, mask_buf, addr_len) == NULL) {
    				return send_message(fd, 1, "Trasnalte mask error\n");
				}
	
				strcat(out_buf, mask_buf);
				strcat(out_buf, "\n");
				return send_message(fd, 1, out_buf);
			}
		}
	}
	
	freeifaddrs(ifaddr);	
	sprintf(out_buf, "Interface %s not found\n", ifs_name);
	return send_message(fd, 1, out_buf);
}

int send_ifs_info(int fd, struct message* m)
{		
	char *command = get_client_command(m->msg);
	char *ifs_names = get_client_ifs_names(m->msg);
	printf("names =\"%s\"\n", ifs_names);
	printf("command =\"%s\"\n", command);

	int result = -1;
	if (ifs_names == NULL || command == NULL) {
		return send_message(fd, 1, "Wrong format\n");
	}

	char *ifs_name = strtok(ifs_names, "-");
	while (ifs_name) {
		if (strcmp(command, "mac") == 0) {
			result = send_ifs_mac(fd, ifs_name);
		} else if (strcmp(command, "status") == 0) {
			result = send_ifs_status(fd, ifs_name);		
		} else if (strcmp(command, "ipv4") == 0) {
			result = send_ifs_ip(fd, ifs_name, AF_INET);
		} else if (strcmp(command, "ipv6") == 0) {
			result = send_ifs_ip(fd, ifs_name, AF_INET6);
		} else {
			result = send_message(fd, 1, "Command is not recognized\n");
		}
		ifs_name = strtok(0, "-");
	}
	
	if (ifs_names != NULL) {
		free(ifs_names);
	}
	return result;
}	
