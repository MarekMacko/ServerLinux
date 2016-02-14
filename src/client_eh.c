#include "client_eh.h"
#include "acceptor_eh.h"
#include "protocol.h"
#include "port_configurator.h"
#include "reactor.h"
#include "if_config.h"
#include <sys/epoll.h>
#include <string.h>

static int handle_client_message(event_handler *self, struct message *m)
{
	int fd = ((a_ctx*)self->ctx)->fd;
	int result = -1;
	
	switch (m->nr) {
		case IF_LIST:
			result = send_ifs_all_names(fd); 
			break;
		case DEV_INFO:
			result = send_ifs_info(fd, m);
			break;
		case SET_PORT:
			result = set_ip(fd, m);	//wymagane odpalenie serwera z sudo
			break;
		case SET_MAC:
			result = set_mac(fd, m);	//wymagane odpalenie serwera z sudo
			break;
		default:
			break;
	}
	delete_message(m);
	return result;
}


static void serve_client(event_handler *self, uint32_t events)
{
	int result = -1;
	struct message *msg;
	int fd = ((a_ctx*)self->ctx)->fd;
	reactor *r = ((a_ctx*)self->ctx)->r;
	
	if (events & EPOLLIN) {
		msg = receive_message(fd);
		if (msg) {
			result = handle_client_message(self, msg);
		}
	} else {
		perror("No epollin events");	
	}
	if (result < 0) {
		r->rm_eh(r, fd);
	}
}


event_handler* construct_client_eh(int fd, reactor *r)
{
	event_handler *eh = malloc(sizeof(event_handler));
	eh->handle_events = serve_client;
	
	a_ctx* ctx = malloc(sizeof(a_ctx));
	ctx->r = r;
	ctx->fd = fd;
	eh->ctx = ctx;
	
	return eh;
}

