#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <stdint.h>

#define MAX_USERS 1000

typedef struct reactor reactor;

typedef struct event_handler{
    int fd;
    reactor* r;
    void (*handle_event)(struct event_handler* self, uint32_t e);
}event_handler;

#endif
