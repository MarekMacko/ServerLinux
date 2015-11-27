#define EPOLLERR 1
#define EPOLLIN 2
#define EPOLLOUT 3

typedef struct event_handler{
	void *ctg;
	int (*get_handler)(struct event_handler *self);
	void (*handle_events)(struct event_handler *self, uint32_t events);	
} event_handler;
