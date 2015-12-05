#ifndef PORT_CONFIGURATOR_H
#define PORT_CONFIGURATOR_H
#include <unistd.h>

int set_ip(const char *deviceName, const char *ip, const char *netmask);
int set_mac(const char *deviceName, const char *mac);

#endif
