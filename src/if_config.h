#include "protocol.h"

int send_ifs_names(int fd);
int send_ifs_status(int fd);
int send_ifs_addr_mac(int fd);
int send_ifs_info(int fd, struct message *m);
static int get_if_ipamask(const char* interface, int family);

