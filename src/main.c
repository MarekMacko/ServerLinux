#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

#include "conf_reader.h"

#define LISTEN_BACKLOG 50

int main(int argc, char **argv)
{
	struct server_settings serv_sett;
	read_settings("config.cfg", &serv_sett);

	return 0;
}

