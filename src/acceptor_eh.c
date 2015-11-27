#include<stdint.h>
#include"acceptor_eh.h"

typedef struct acceptor_ctx{
	int fd;
	reactor *r;
}a_ctx;

static int get_handle(event_handler *self){
	return (self && self->ctx ? ((a_ctx *)self->ctx)->fd : -1);
}

static void accept_cli(event_handle *self, uint32_t){
	int fd = self->get_handle(self);
	int c_fd = -1;
	event_handler c_eh;

	struct sockaddr addr;
	socklen_t addr_len = sizeof(addr);
	if(es & EPOLLIN){
		c_fd = acceptr(fd, &addr, &addr_len);
		c_eh = construc_client_eh(     c_fd);
