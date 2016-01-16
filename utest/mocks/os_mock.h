extern "C" {
	#include "os.h"
}

#include <cmock/cmock.h>

DECLARE_FUNCTION_MOCK3(os_socket_mock, os_socket, int(int, int, int));
DECLARE_FUNCTION_MOCK3(os_bind_mock, os_bind, int(int, const struct sockaddr *, socklen_t));
DECLARE_FUNCTION_MOCK2(os_listen_mock, os_listen, int(int, int));
DECLARE_FUNCTION_MOCK3(os_accept_mock, os_accept, int(int, struct sockaddr *, socklen_t *));
DECLARE_FUNCTION_MOCK3(os_read_mock, os_read, ssize_t(int, void *, size_t));
DECLARE_FUNCTION_MOCK3(os_write_mock, os_write, ssize_t(int, const void *, size_t));
DECLARE_FUNCTION_MOCK1(os_close_mock, os_close, int(int));

DECLARE_FUNCTION_MOCK4(os_epoll_ctl_mock, os_epol_ctl, int(int, int, int, struct epoll_event *));
DECLARE_FUNCTION_MOCK4(os_epoll_wait_mock, os_wait_ctl, int(int, struct epoll_event *, int, int));
