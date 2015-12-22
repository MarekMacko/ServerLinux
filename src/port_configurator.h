#ifndef PORT_CONFIGURATOR_H
#define PORT_CONFIGURATOR_H

#include <unistd.h>
#include "protocol.h"
int set_ip(int fd, struct message* m);
int set_mac(int fd, struct message* m);

#endif
