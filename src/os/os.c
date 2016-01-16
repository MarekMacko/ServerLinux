#include "os.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

int os_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
}

int os_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	return bind(sockfd, addr, addrlen);
}

int os_listen(int sockfd, int backlog)
{
	return listen(sockfd, backlog);
}

int os_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return accept(sockfd, addr, addrlen);
}

ssize_t os_read(int fd, void *buf, size_t count)
{
	return read(fd, buf, count);
}

ssize_t os_write(int  fd,  const  void  *buf, size_t count)
{
	return write(fd,  buf, count);
}

int os_close(int fd)
{
	return close(fd);
}

int os_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
	return epoll_ctl(epfd, op, fd, event);
}

int os_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
	return epoll_wait(epfd, events, maxevents, timeout);
}
