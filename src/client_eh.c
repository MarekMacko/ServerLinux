#include "client_eh.h"
#include "acceptor_eh.h"
#include "protocol.h"
#include "port_configurator.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

static void serve_client(event_handler* self, uint32_t events)
{
	struct message *msg;
	int fd = ((a_ctx*) self->ctx)->fd;
	if (events & EPOLLIN) {
		msg=receive_message(fd);
		//printf("%s\n",msg->msg);
	} else {
//		
	}
	perror("No events\n");
	switch(msg->nr){
		case IF_LIST:
			printf("Lista interface obsluga\n");
			break;
		case DEV_INFO:
			printf("Info o danym interface\n");
			break;
		case SET_PORT:
			if(set_ip("eth1","192.10.0.1","255.255.0.0"))	//wymagane odpalenie serwera z sudo
				send_message(fd, 1, "Blad podczas ustawiania adresu ip i maski");
			else
				send_message(fd, 1, "Adres ip zostal poprawnie ustawiony");
			break;
		case SET_MAC:
			if(set_mac("eth1","12:91:78:56:34:12"))	//wymagane odpalenie serwera z sudo
				send_message(fd, 1, "Blad podczas ustawiania mac adresu");
			else
				send_message(fd, 1, "Adres mac zostal poprawnie ustawiony");
			break;
		default:
			send_message(fd, 1, "Nieznana komenda, serwer nie może jej obsluzyc");
	}
	delete_message(msg);
	//free(msg);
}

event_handler* construct_client_eh(int fd, reactor *r)
{
	event_handler *eh = malloc(sizeof(event_handler));
	eh->handle_events = serve_client;	
	((a_ctx*) eh->ctx)->fd = fd;
	((a_ctx*) eh->ctx)->r = r;
	
	return eh;
}

