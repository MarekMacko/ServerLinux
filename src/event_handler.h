//#define EPOLLERR 1
//#define EPOLLIN 2
//#define EPOLLOUT 3

#include <stdint.h>

typedef struct reactor reactor;

typedef struct event_handler{
	int fd;
	reactor *r;
    void *ctg;
    int (*get_handler)(struct event_handler *self);
    void (*handle_event)(struct event_handler* self, uint32_t e);	
} event_handler;
