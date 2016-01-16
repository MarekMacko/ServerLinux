extern "C" {
	#include "acceptor_eh.h"
	#include "client_eh.h"
	#include "reactor.h"
}

#include "mocks/os_mock.h"

#include "<gtest/gtest.h"

using namespace ::testing;

TEST(server_test,  

reactor *r = 0;
event_handler* serv_eh = 0;

r = create_reactor(ss.max_clients);
if (r == 0) {
	return -1;
}

serv_eh = construct_acceptor(r, &ss);
if (serv_eh == 0) {
	destroy_reactor(r);
	return -1;
}

signal(SIGPIPE, SIG_IGN);
r->add_eh(r, serv_eh);
r->event_loop(r);
free(r);
free(serv_eh);
}
