#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <stdint.h>

typedef struct reactor reactor;

typedef struct event_handler {
    void* ctx;
	int (*get_handle)(struct event_handler* self);
    void (*handle_events)(struct event_handler* self, uint32_t e);
} event_handler;

#endif
