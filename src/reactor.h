#define MAX_IDX (MAX_USERS-1)

typedef struct reactor_core {
    int epoll_fd;
    ssize_t current_idx;
    event_handler* ehs[MAX_IDX];
} reactor_core;

typedef struct reactor {
    void (*add_eh)(reactor* self, event_handler* eh);
    void (*rm_eh)(reactor* self, inf fd);
    void (*event_loop)(reactor* self);
    reactor_core* rc;
} reactor;

reactor* create_reactor(int epoll_fd);
void destroy_reactor(reactor *r);
