#include <stdint.h>
#include "acceptor_eh.h"
#include "reactor.h"
#include "client_eh.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "os/os.h"

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
	
	if (es & EPOLLIN) {
		c_fd = os_accept(fd, 0, 0);
		c_eh = construct_client_eh(c_fd, r);
		r->add_eh(r, c_eh);
	}
}

event_handler* construct_acceptor(reactor* r, serv_sett* ss) 
{
	int serv_fd = -1; 
	struct epoll_event ee; 
	struct sockaddr_in serv_addr;
	event_handler* eh;
	a_ctx* ctx;
	
	serv_fd = os_socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	if(serv_fd < 0) {
		perror("construct_acceptor socket");
		return 0; 
    }   
													    
	memset(&ee, 0, sizeof(ee));													
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(ss->port);
										    
	if(os_bind(serv_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("construct_acceptor bind");
		close(serv_fd);
		return 0; 
    }  		
	
	if(os_listen(serv_fd, LISTEN_BACKLOG) < 0) {
		perror("construct_acceptor listen");
		close(serv_fd);
		return 0; 
	}												

	ctx = malloc(sizeof(a_ctx));
	ctx->r = r;
	ctx->fd = serv_fd;

	eh = malloc(sizeof(event_handler));
	eh->ctx = ctx;
	eh->get_handle = get_handle;
	eh->handle_events = accept_cli;

	return eh;
}
