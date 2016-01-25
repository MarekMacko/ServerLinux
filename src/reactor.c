#include "if_config.h"
#include "reactor.h"
#include "acceptor_eh.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "os/os.h"

static event_handler* find_eh(reactor_core* rc, int fd, size_t* idx)
{
	size_t i = 0;
	event_handler* eh = 0;
	for (i = 0; i <= rc->current_idx; i++) {
		if(rc->ehs[i] && (((a_ctx*)rc->ehs[i]->ctx)->fd == fd)) {
			eh = rc->ehs[i];
            if(idx) {
            	*idx = i;
			}
            break;
    	}
	}
	return eh;
}

static void add_eh(reactor* self, event_handler* eh)
{
    struct epoll_event ee;
    int fd = ((a_ctx*)eh->ctx)->fd; 

    if(self->rc->current_idx < self->rc->max_cli) {
		memset(&ee, 0, sizeof(ee));
    	ee.events = EPOLLIN;
    	ee.data.fd = fd;
    	os_epoll_ctl(self->rc->epoll_fd, EPOLL_CTL_ADD, fd, &ee);
        printf("New client with fd=%d accepted\n", fd); 
		if((self->rc->current_idx == 0) && (self->rc->ehs[0] == 0)) {
            self->rc->ehs[0] = eh;
	    } else {
            self->rc->ehs[++(self->rc->current_idx)] = eh;
		}
	} else {
		send_message(fd, 1, "To many clients\n");
		close(fd);
		free(eh);
		printf("To many clients\n");
	}
}

static void rm_eh(reactor* self, int fd)
{
    size_t i = 0;
    event_handler* eh = find_eh(self->rc,fd, &i);
    if (!eh) {
        printf("Removing client with fd %d failed\n", fd);
		return;
    }
	
    if (self->rc->current_idx > i)
        self->rc->ehs[i] = self->rc->ehs[self->rc->current_idx];

    self->rc->ehs[self->rc->current_idx] = 0;
    if (self->rc->current_idx > 0) {
        --(self->rc->current_idx);
    }
    
    os_epoll_ctl(self->rc->epoll_fd, EPOLL_CTL_DEL, ((a_ctx*)eh->ctx)->fd, 0);
    close(eh->get_handle(eh));
    free(eh);

    printf("Removing client with fd %d success\n", fd);
}

static void event_loop(reactor* self)
{
    int i = 0;
    int epoll_fd = self->rc->epoll_fd;
    struct epoll_event es[self->rc->max_cli];
    event_handler* eh = 0;
    
	for(;;) {
        i = os_epoll_wait(epoll_fd, es, self->rc->max_cli, -1);  //czeka na event
        if (i < 0) {
            printf("Epoll error: %d\n", i);
            break;
        }
		for (--i; i >-1; --i) {
            eh = find_eh(self->rc, es[i].data.fd, 0);
			if (eh) {
                eh->handle_events(eh, es[i].events);
			}
        }
    }
}

reactor* create_reactor(int max_cli)
{
    reactor* r = 0;
    int i;
    int epoll_fd = os_epoll_create(max_cli + 1);
    if (epoll_fd < 0){
        perror("epoll_create"); 
        return 0;
    }

    r = malloc(sizeof(reactor));
    r->rc = malloc(sizeof(reactor_core));
    r->rc->ehs = malloc(sizeof(event_handler*) * (max_cli + 1));
	r->rc->max_cli = max_cli;
	
	for (i = 0; i < max_cli; i++) {
		r->rc->ehs[i] = 0;
	}

    r->rc->epoll_fd = epoll_fd;
    r->rc->current_idx = 0;
    r->add_eh = &add_eh;
    r->rm_eh = &rm_eh;
    r->event_loop = &event_loop;

    return r;
}

void destroy_reactor(reactor* r) {
	free(r->rc->ehs);	
    free(r->rc);
    free(r);
}
    


