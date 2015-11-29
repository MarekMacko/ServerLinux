#ifndef CONF_READER_H
#define CONF_READER_H
#include <libconfig.h>

typedef struct server_settings{
	int port;
	int max_clients;
} serv_sett;

int read_settings(const char*, serv_sett* );

#endif
