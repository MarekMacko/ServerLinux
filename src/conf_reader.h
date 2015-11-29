#ifndef CONF_READER_H
#define CONF_READER_H
#include <libconfig.h>

typedef struct server_settings{
	int port;
	int max_clients;
}server_sett;
void read_settings(const char *, struct server_settings *);
#endif