extern "C" {
	#include "conf_reader.h"
	#include "acceptor_eh.h"
	#include "protocol.h"
	#include "if_config.h"
	#include "client_eh.h"
	#include "reactor.h"
}

#include "mocks/os_mock.h"

#include <gtest/gtest.h>

using namespace ::testing;

TEST(server_test, epoll_wait_fails_first_time)
{
	int srv_fd = 1;
	int epoll_fd = 2;
	
	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	reactor *r = 0;
	event_handler* serv_eh = 0;

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	os_epoll_ctl_mock ecn;
	os_epoll_wait_mock ewm;

	EXPECT_FUNCTION_CALL(ecn, (epoll_fd, _, srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).WillOnce(Return(-1));

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	signal(SIGPIPE, SIG_IGN);
	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}
