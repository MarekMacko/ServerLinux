#include "client_eh.h"
#include "acceptor_eh.h"
#include "protocol.h"
#include "port_configurator.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include "reactor.h"

static int handle_client_message(event_handler* self, struct message* m)
{
	int fd = ((a_ctx*)self->ctx)->fd;
	int result = -1;

	switch(m->nr){
		case IF_LIST:
			printf("Lista interface obsluga\n");
			result = 1;
			break;
		case DEV_INFO:
			printf("Info o danym interface\n");
			result = 1;
			break;
		case SET_PORT:
			if(set_ip("eth1","192.10.0.1","255.255.0.0"))	//wymagane odpalenie serwera z sudo
				send_message(fd, 1, "Blad podczas ustawiania adresu ip i maski");
			else
				send_message(fd, 1, "Adres ip zostal poprawnie ustawiony");
			result = 1;
			break;
		case SET_MAC:
			if(set_mac("eth1","12:91:78:56:34:12"))	//wymagane odpalenie serwera z sudo
				send_message(fd, 1, "Blad podczas ustawiania mac adresu");
			else
				send_message(fd, 1, "Adres mac zostal poprawnie ustawiony");
			result = 1;
			break;
		default:
			send_message(fd, 1, "Nieznana komenda, serwer nie może jej obsluzyc");
			result = 1;
	}
	delete_message(m);
	return result;

}



static void serve_client(event_handler* self, uint32_t events)
{
	printf("serve client\n");
	int result = -1;
	struct message *msg;
	int fd = ((a_ctx*)self->ctx)->fd;
	printf("po fd\n");
	if ( (events & EPOLLERR) || (events & EPOLLET) || (events & EPOLLOUT) || (events & EPOLLPRI) || (events & EPOLLRDHUP) || (events & EPOLLONESHOT) ||  (events & EPOLLHUP) )
		printf("Disconnected\n");

	if (events & EPOLLIN) {
		printf("events & EPOLLIN\n");
		msg = receive_message(fd);
		printf("Serve_client received message: %s \n", msg->msg);

		if (msg)
			result = handle_client_message(self, msg);
	} else {
		printf("No EPOLLIN events\n");	
		perror("No EPOLLIN events\n");	
	}
	printf("check result \n");
	if (result < 0) {
//		reactor* r = ((a_ctx*)self->ctx)->r; 
//		r->rm_eh(((a_ctx*)self->ctx)->r, ((a_ctx*)self->ctx)->fd);
		printf("result < 0 have to remove event handler\n");
		((reactor*)((a_ctx*)self->ctx)->r)->rm_eh(((a_ctx*)self->ctx)->r, ((a_ctx*)self->ctx)->fd);
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

