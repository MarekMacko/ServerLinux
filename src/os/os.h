#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

int os_socket(int domain, int type, int protocol);
int os_bind(int sockfd, const struct sockaddr *addr, socklen_t adrlen);
int os_listen(int sockfd, int backlog);
int os_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
ssize_t os_read(int fd, void *buf, size_t count);
int os_close(int fd);
int os_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int os_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
