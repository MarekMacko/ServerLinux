#ifndef ACCEPTOR_EH_H
#define ACCEPTOR_EH_H

#include "event_handler.h"
#include "conf_reader.h"

typedef struct acceptor_ctx { 
	int fd; 
    reactor* r; 
} a_ctx;

struct event_handler* construct_acceptor(reactor* r, serv_sett* ss);
void destruct_acceptor(event_handler* eh);

#endif
