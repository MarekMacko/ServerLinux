#include "reactor.h"
#include "acceptor_eh.h"
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static event_handler* find_eh(reactor_core* rc, int fd, size_t* idx)
{
	size_t i = 0;
	event_handler* eh = 0;
	for(i = 0; i <= rc->current_idx; i++){
		if(rc->ehs[i] && (((a_ctx*)rc->ehs[i]->ctx)->fd == fd)){
        	eh = rc->ehs[i];
            if(idx)
            	*idx = i;
            break;
    	}
	}

	return eh;
}

static void add_eh(reactor* self, event_handler* eh)
{
    struct epoll_event ee;
    memset(&ee, 0, sizeof(ee));
    ee.events = EPOLLIN;
    ee.data.fd = ((a_ctx*)eh->ctx)->fd;
    epoll_ctl(self->rc->epoll_fd,EPOLL_CTL_ADD, ((a_ctx*)eh->ctx)->fd, &ee);

    if(self->rc->current_idx < self->rc->max_cli - 1) {
        if((self->rc->current_idx == 0) && (self->rc->ehs[0] == 0)) {
            self->rc->ehs[0] = eh;
       		printf(" Eh added to start\n");
	    } else {
            self->rc->ehs[++(self->rc->current_idx)] = eh;
    		printf(" Eh added to pos %d\n",(int)self->rc->current_idx);
		}
	}
}

static void rm_eh(reactor* self, int fd)
{
   	printf("Remove event handler\n");
    size_t i = 0;
    event_handler* eh = find_eh(self->rc,fd, &i);
    if(!eh)
        return;
    
    if(i<self->rc->current_idx)
        self->rc->ehs[i]=self->rc->ehs[self->rc->current_idx];

    self->rc->ehs[self->rc->current_idx]=0;
    if(self->rc->current_idx>0){
        --(self->rc->current_idx);
    }
    
    epoll_ctl(self->rc->epoll_fd, EPOLL_CTL_DEL, ((a_ctx*)eh->ctx)->fd,0);
    close(((a_ctx*)eh->ctx)->fd);
    free(eh);
}

static void event_loop(reactor* self)
{
    int i = 0;
    int epoll_fd = self->rc->epoll_fd;
    struct epoll_event es[self->rc->max_cli];
    event_handler* eh = 0;
    
	while(1) {
        i = epoll_wait(epoll_fd, es, self->rc->max_cli, -1);  //czeka na event    
		for (--i; i >-1; --i) {
            eh = find_eh(self->rc, es[i].data.fd, 0);
			printf("Finded eh fd %d:\n", ((a_ctx*)eh->ctx)->fd);
			if (eh)
                eh->handle_events(eh, es[i].events);
        }
    }
}

reactor* create_reactor()
{
    reactor* r = malloc(sizeof(reactor));
    r->rc = (reactor_core*) malloc(sizeof(reactor_core));
    r->rc->ehs = malloc(sizeof(event_handler*));

    r->rc->epoll_fd = 0; // set in construct_acceptor
    r->rc->current_idx = 0;
    r->add_eh = &add_eh;
    r->rm_eh = &rm_eh;
    r->event_loop = &event_loop;

    return r;
}

void destroy_reactor(reactor* r){
    free(r->rc);
    free(r);
}
    


