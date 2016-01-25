#include "os_mock.h"

IMPLEMENT_FUNCTION_MOCK3(os_socket_mock, os_socket, int(int, int, int));
IMPLEMENT_FUNCTION_MOCK3(os_bind_mock, os_bind, int(int, const struct sockaddr *, socklen_t));
IMPLEMENT_FUNCTION_MOCK2(os_listen_mock, os_listen, int(int, int));
IMPLEMENT_FUNCTION_MOCK3(os_accept_mock, os_accept, int(int, struct sockaddr *, socklen_t *));
IMPLEMENT_FUNCTION_MOCK3(os_read_mock, os_read, ssize_t(int, void *, size_t));
IMPLEMENT_FUNCTION_MOCK3(os_write_mock, os_write, ssize_t(int, const void *, size_t));
IMPLEMENT_FUNCTION_MOCK1(os_close_mock, os_close, int(int));
IMPLEMENT_FUNCTION_MOCK1(os_epoll_create_mock, os_epoll_create, int(int));
IMPLEMENT_FUNCTION_MOCK4(os_epoll_ctl_mock, os_epoll_ctl, int(int, int, int, struct epoll_event *));
IMPLEMENT_FUNCTION_MOCK4(os_epoll_wait_mock, os_epoll_wait, int(int, struct epoll_event *, int, int));
