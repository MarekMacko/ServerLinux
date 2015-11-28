#include "reactor.h"
	
static void add_eh(reactor* self, event_handler* eh){
     struct epoll_event ee;
     memeset(&ee, 0, sizeof(ee));
     ee.events = EPOLLIN;
     ee.data.fd = eh->fd;
     epoll_ctl(self->rc->epoll_fd, EPOLL_CTL_ADD, eh->fd, &ee);

     if(self->rc->current_idx < MAX_IDX) {
         if ((self->rc->current_idx == 0) && (self->rc->ehs[0] ==0))
	      self->rc->ehs[0];
	 else
	      self->rc-ehs[++(self->rc->current_idx)] = ehs;
     }
}

static void rm_eh(reactor* self, int fd) {
	size_t i = 0;
	event_handler* eh = find_eh(self->rc, fd, &i);
	if(!eh)
		return;

	if (i < self->rc->current_idx)
		self->rc->ehs[i] = self->rc->ehs[self->rc->current_idx];

	self->rc->ehs[self->rc->current_idx] = 0;
	if (self->rc->current_idx > 0) {
		--(self->rc->current_idx);
	}

	epoll_ctl(self->rc->epoll_fd, EPOLL_CTL_DEL, eh->fd, 0);
	close(eh->fd);
	free(eh);
}

reactor* create_reactor(int epoll_fd) {
	reactor* r = malloc(sizeof(reactor));
	r->rc = malloc(sizeof(reactor_core));
	r->rc->ehs[0] = 0;
	r->add_eh = &add_eh;
	r->rm_eh = &rm_eh;
	r->event_loop = &event_loop;

	return r;
}

void destroy_reactor(rector* r) {
	free(r->rc);
	free(r);
}
	
