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

#define MAXIFS 10 // do zmiany 
#define IF_LEN_NAME 10 // do zmiany

int send_ifs_info(int fd, struct message* m, enum message_type mes_type)
{
	struct ifaddrs *ifaddr, *ifa;
	char ifs_all_names[MAXIFS*IF_LEN_NAME];
	int family;
	char* msg = m->msg;
	char* ifs_list = NULL; // list of ifs client names 
	char* end_ifs_list = NULL;
	bool whole_ifs_list = false; // is client need to all list of interaces
	
	end_ifs_list = strchr(msg, ';');
	ifs_list = strtok(msg, ";"); 

	// if client don't set command for interface
	if ((end_ifs_list == 0) && (mes_type != IF_LIST)) {
		return send_message(fd, 1, "You must set one of this command \n-status\n-mac\n-ipv4\n-ipv6");
	}

	if (strcmp(ifs_list, "all") == 0) {
		whole_ifs_list = true;
	}

	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(1);
	}
	
	family = ifaddr->ifa_addr->sa_family;

	// if client want to get list of interfaces or specific information about all of them 
	if ((mes_type == IF_LIST) || (whole_ifs_list == true)) {
		//in both cases we need to get list with all names
		memset(ifs_all_names, 0, sizeof(ifs_all_names));	
		
		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == NULL) {
				continue;
			}
			if (ifa->ifa_addr->sa_family == family) {
				strcat(ifs_all_names, ifa->ifa_name);
				strcat(ifs_all_names, "\n");
			}
		}
		
		// if client want only list with names then return with send
		if (mes_type == IF_LIST) {
			freeifaddrs(ifaddr);
			return send_message(fd, 1, ifs_all_names);
		}
	}
	
	// if client want to get secific information about interface
	if (mes_type == DEV_INFO) {
		char ifs_separ[2]; // separator in interaces list
		char* command; // longest command name "status"	
//		socklen_t addr_len = (family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;//16 or 46
		socklen_t addr_len = INET6_ADDRSTRLEN;
		char addr_buf[addr_len];
		char mask_buf[addr_len];
		char mac_buf[18];
		char* ifs_name; // single interface name
		char out_buf[50];
		void* addr;
		bool status = false;
		bool mac = false;

		// check how many interfaces client want to test
		if (whole_ifs_list == true) {
			strcpy(ifs_separ, "\n");
			ifs_list = ifs_all_names;
		} else {
			strcpy(ifs_separ, "-");
		}
		
	 	command = malloc(sizeof(char)*7); // longest "status" + 1 	
		strcpy(command, ++end_ifs_list);

		ifs_name = strtok(ifs_list, ifs_separ);
		
		if (strcmp(command, "ipv4") == 0) {
			family = AF_INET;
		} else if (strcmp(command, "ipv6") == 0) {
			family = AF_INET6;
		} else if (strcmp(command, "status") == 0) {
			status = true;
		} else if (strcmp(command, "mac") == 0) {
			mac = true;
		} else {
			free(command);
			return send_message(fd, 1, "Command is not recognized\n");
		}
			
		while(ifs_name != 0) {	
			memset(out_buf, 0, sizeof(out_buf));
			memset(addr_buf, 0, sizeof(addr_buf));
			memset(mask_buf, 0, sizeof(mask_buf));
		
			for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
				if (ifa->ifa_addr == NULL)
					continue;
		
				if (strcmp(ifa->ifa_name, ifs_name) == 0) {	
					if (status == true) {
						if (ifa->ifa_addr->sa_family == family) {
							strcat(out_buf, ifs_name);
							strcat(out_buf, "\t");
							if (ifa->ifa_flags & IFF_RUNNING) {
								strcat(out_buf, "UP\n");
							} else {
								strcat(out_buf, "DOWN\n");
							}
							break;
						}
					} else if (mac == true) {
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
							break;
						}	
					} else if (ifa->ifa_addr->sa_family == family) {
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
						break;
					}
				}
			}
			if (out_buf[0] != 0) {
				send_message(fd, 1, out_buf);
			}
			ifs_name = strtok(0, ifs_separ);
		}		
		free(command);
//		freeifaddrs(ifaddr);
	}

	if (ifaddr != NULL) {
		freeifaddrs(ifaddr);
	}
    return 0;
}
