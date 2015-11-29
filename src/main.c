#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "conf_reader.h"
#include "reactor.h"

#define LISTEN_BACKLOG 10

int init_server(int* s, int* e, serv_sett* ss);

int main(int argc, char **argv)
{
	int srv_fd = -1;
	int epoll_fd = -1;
	serv_sett ss;

	read_settings("config.cfg", &ss);
	
	reactor *r = 0;
	event_handler* seh = 0; 

	if (init_server(&srv_fd, &epoll_fd, &ss) < 0){
		return -1;
	}
	printf("Init sucessfully\n");
	r->rc->max_cli;
	r = create_reactor(epoll_fd);
//	seh = construct_acceptor(int fd, 
	return 0;
}

int init_server(int* s, int* e, serv_sett* ss) {

	int serv_fd = -1;
	int epoll_fd = -1;
	struct epoll_event ee;
	struct sockaddr_in serv_addr;
			        						    
	serv_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(serv_fd < 0) {
		perror("socket");
		return -1;
	}
													    
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(ss->port);
												    
	if(bind(serv_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("socket");
		close(serv_fd);
		return -1;
	}
	
	if(listen(serv_fd, LISTEN_BACKLOG) < 0) {
		perror("listen");
		close(serv_fd);
		return -1;
	}

	epoll_fd = epoll_create(ss->max_clients + 1);
	if (epoll_fd < 0){
		perror("epoll_create");
		close(serv_fd);
		return -1;
	}

	ee.events = EPOLLIN;
	ee.data.fd = serv_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_fd, &ee) < 0) {
		perror("epoll_ctl");
		close(epoll_fd);
		close(serv_fd);
		return -1;
	}

	*s = serv_fd;
	*e = epoll_fd;

	return 0;
}

