#include "client_eh.h"
#include "acceptor_eh.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

static void serve_client(event_handler* self, uint32_t events)
{
//	size_t len = 0;
	int size = 10;
	char buff[size];
	int fd = ((a_ctx*) self->ctx)->fd; 
	if (events & EPOLLIN) {
		if (read(fd, &buff, size) > 0)
			printf("Readed data: %s\n", buff);
	} else {
//		perror("No events\n");
	}
}

event_handler* construct_client_eh(int fd, reactor *r)
{
	event_handler *eh = malloc(sizeof(event_handler));
	eh->handle_events = serve_client;	
	((a_ctx*) eh->ctx)->fd = fd;
	((a_ctx*) eh->ctx)->r = r;
	
	return eh;
}

