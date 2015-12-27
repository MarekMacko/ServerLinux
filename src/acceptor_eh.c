#include <stdint.h>
#include "acceptor_eh.h"
#include "reactor.h"
#include "client_eh.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define LISTEN_BACKLOG 10

static int get_handle(event_handler *self)
{
	return (self && self->ctx ? ((a_ctx *)self->ctx)->fd : -1);
}

static void accept_cli(event_handler *self, uint32_t es)
{	
	int c_fd = -1;
	int fd = self->get_handle(self);
	reactor* r = ((a_ctx*) self->ctx)->r;
	event_handler* c_eh;
	
	struct sockaddr addr;
	socklen_t addr_len = sizeof(addr);
	if(es & EPOLLIN){
		c_fd = accept(fd, &addr, &addr_len);
		c_eh = construct_client_eh(c_fd, r);
	 	r->add_eh(r, c_eh);
		printf("New client with fd=%d accepted\n", ((a_ctx*)c_eh->ctx)->fd);
	}
}

event_handler* construct_acceptor(reactor* r, serv_sett* ss) 
{
	int serv_fd = -1; 
	int epoll_fd = -1; 
	struct epoll_event ee; 
	struct sockaddr_in serv_addr;
	event_handler* eh;
	a_ctx* ctx;
	
	serv_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	if(serv_fd < 0) {
		perror("construct_acceptor socket");
		return 0; 
    }   
													    
	memset(&ee, 0, sizeof(ee));													
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(ss->port);
										    
	if(bind(serv_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("construct_acceptor bind");
		close(serv_fd);
		return 0; 
    }  		
	
	if(listen(serv_fd, LISTEN_BACKLOG) < 0) {
		perror("construct_acceptor listen");
		close(serv_fd);
		return 0; 
	}											
	
	epoll_fd = epoll_create(ss->max_clients + 1);
	if (epoll_fd < 0){
		perror("epoll_create");	
		close(serv_fd);
		return 0;
	}   
    
	ee.events = EPOLLIN;
    ee.data.fd = serv_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_fd, &ee) < 0) {
		perror("epoll_ctl");
        close(epoll_fd);
        close(serv_fd);
        return 0;
    }

	r->rc->epoll_fd = epoll_fd;	

	ctx = malloc(sizeof(a_ctx));
	ctx->r = r;
	ctx->fd = serv_fd;

	eh = malloc(sizeof(event_handler));
	eh->ctx = ctx;
	eh->get_handle = get_handle;
	eh->handle_events = accept_cli;

	return eh;
}
