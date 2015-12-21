#ifndef REACTOR_H
#define REACTOR_H

#include "event_handler.h"
#include <stdlib.h>

typedef struct reactor_core {
	int epoll_fd;
    size_t current_idx;
    int max_cli;
	event_handler** ehs;
} reactor_core; 

typedef struct reactor {
	void (*add_eh)(reactor* self, event_handler* eh);
	void (*rm_eh)(reactor* self, int fd);
	void (*event_loop)(reactor* self);
	reactor_core* rc;
} reactor;
 
reactor* create_reactor(int max_cli);
void destroy_reactor(reactor* r);

#endif 
