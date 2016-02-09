extern "C" {
	#include "conf_reader.h"
	#include "acceptor_eh.h"
	#include "protocol.h"
	#include "if_config.h"
	#include "client_eh.h"
	#include "reactor.h"
	//#include "port_configurator.h"
}

#include "mocks/os_mock.h"

#include <gtest/gtest.h>

using namespace ::testing;

TEST(server_test, epoll_wait_fails_first_time)
{
	int srv_fd = 1;
	int epoll_fd = 4;
	
	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;

	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).WillOnce(Return(-1));

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}

TEST(server_test, accept_client_and_disconnect)
{
	int srv_fd = 1;
	int cli_fd = 2;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLERR;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client	3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));
	// 1: reactor->add_eh 		2: return -1
	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}


TEST(construct_acceptor_test, socket_failed)
{
	reactor *r = 0;
	event_handler *serv_eh;
	
	
	os_socket_mock osm;

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(-1));

	r = create_reactor(1);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, NULL);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}
	
	ASSERT_TRUE(true);
}

TEST(construct_acceptor_test, bind_failed)
{
	reactor *r = 0;
	event_handler *serv_eh;
	int serv_fd = 2;
	
	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	os_socket_mock osm;
	os_bind_mock obm;

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(serv_fd));
	EXPECT_FUNCTION_CALL(obm, (_,_,_)).WillOnce(Return(-1));

	r = create_reactor(1);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}
	
	ASSERT_TRUE(true);
}


TEST(construct_acceptor_test, listen_failed)
{
	reactor *r = 0;
	event_handler *serv_eh;
	int srv_fd = 2;
	
	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;

	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _,_)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(-1));

	r = create_reactor(1);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}
	
	ASSERT_TRUE(true);
}

ACTION_P(read_size, size) 
{
	memcpy(arg1, (void*)size, sizeof(size_t));
	return sizeof(size_t);
}

ACTION_P(read_msg, msg)
{
	memcpy(arg1, msg, strlen(msg));
	return strlen(msg);
}

TEST(read_and_send_test, interfaces_list)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "1;";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}

TEST(read_and_send_test, interfaces_macs)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "2;all;mac";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}


TEST(read_and_send_test, interfaces_ipv4)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "2;all;ipv4";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}


TEST(read_and_send_test, interfaces_ipv6)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "2;all;ipv6";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}


TEST(read_and_send_test, interfaces_status)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "2;all;status";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}

TEST(read_and_set, ip)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "3;eth0;100.100.100.100;255.255.255.255";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}


TEST(read_and_set, mac)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "4;eth0;AA:AA:AA:AA:AA:AA";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}


TEST(read_and_set, wrong_message_key)
{
	int srv_fd = 5;
	int cli_fd = 6;
	int epoll_fd = 4;

	size_t len;

	struct epoll_event e_srv;
	e_srv.events = EPOLLIN;
	e_srv.data.fd = srv_fd;

	struct epoll_event e_cli;
	e_cli.events = EPOLLIN;
	e_cli.data.fd = cli_fd;

	serv_sett ss;
	ss.port = 3000;
	ss.max_clients = 10;

	const char *msg = "9;";
	size_t msg_len = strlen(msg);
		
	reactor *r = 0;
	event_handler* serv_eh = 0;

	os_epoll_create_mock ecrem;
	os_epoll_ctl_mock ecm;
	os_epoll_wait_mock ewm;
	
	os_socket_mock osm;
	os_bind_mock obm;
	os_listen_mock olm;
	os_accept_mock oam;

	os_read_mock orm;


	// create reactor
	EXPECT_FUNCTION_CALL(ecrem, (ss.max_clients+1)).WillOnce(Return(epoll_fd));
	// 1: add_ed for server 	2: add_ed for client 3: rm_eh for client
	EXPECT_FUNCTION_CALL(ecm, (epoll_fd, _, _, _)).Times(3)\
													.WillRepeatedly(Return(0));

	EXPECT_FUNCTION_CALL(ewm, (epoll_fd, _, _, -1)).Times(3)\
									.WillOnce(DoAll(SetArgPointee<1>(e_srv), Return(1)))\
									.WillOnce(DoAll(SetArgPointee<1>(e_cli), Return(1)))\
									.WillOnce(Return(-1));

	// construct acceptor
	EXPECT_FUNCTION_CALL(osm, (_,_,_)).WillOnce(Return(srv_fd));
	EXPECT_FUNCTION_CALL(obm, (srv_fd, _, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(olm, (srv_fd, _)).WillOnce(Return(0));
	EXPECT_FUNCTION_CALL(oam, (srv_fd, 0, 0)).WillOnce(Return(cli_fd));

	EXPECT_FUNCTION_CALL(orm, (cli_fd, _,_)).Times(2)\
									.WillOnce(read_size(&msg_len))\
									.WillOnce(read_msg(msg));

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return;
	}

	serv_eh = construct_acceptor(r, &ss);
	if (serv_eh == 0) {
		destroy_reactor(r);
		return;
	}

	r->add_eh(r, serv_eh);
	r->event_loop(r);
	free(r);
	free(serv_eh);

	ASSERT_TRUE(true);
}

