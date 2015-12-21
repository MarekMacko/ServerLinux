#include "protocol.h"
#include <unistd.h>
#include <string.h>


static struct message_key {
	enum message_type id;
	const char *str;
} message_key[] = {		//polecenia nie mogą być dłuższe niż 20 znaków
						{ ACK_NACK, "ack_nack" },
						{ IF_LIST, "if_list" },
						{ DEV_INFO, "get_info" },
						{ SET_PORT, "set_ip" },
						{ SET_MAC, "set_mac" }
};

static int send_bytes(int fd, const char* msg, size_t len)
{
	int result = -1;
	if (write(fd, &len, sizeof(size_t)) > 0)
		if (write(fd, msg, len) == len)
			result = 0;

	return result;
}

int parse_message_key(const char *str)
{
	int n_keys = sizeof(message_key) / sizeof(message_key[0]);
	int i;
	for (i = 0; i < n_keys; i++)
		if (strcmp(str, message_key[i].str) == 0)
			return message_key[i].id;

	return ACK_NACK;	//lub najlepiej jakiś error
}

int send_message(int fd, bool is_message, const char* error_msg)
{
	char* msg = "0;";
	size_t len = 2;
	if (is_message) {
		len = strlen(error_msg) + 2;
		msg = malloc((len + 1) * sizeof(char));
		strcpy(msg, "0;");
		strcpy(msg+2, error_msg);
	}

	return send_bytes(fd, msg, len);
}

int send_message_to_server(int fd, const char* msg, size_t len)
{
	send_bytes(fd,msg,len);
	return 0;
}

int send_dev_info(int fd)
{

	return 0;
}

struct message* receive_message(int fd)
{
	size_t len;
	int readed;
	char* msg = 0;
	struct message* m = 0;

	readed = read(fd, &len, sizeof(size_t));
	if (readed < 1)
		return 0;

	msg = malloc(len * sizeof(char));
	if (read(fd, msg, len) != len) {
		return 0;
	}

	m = malloc(sizeof(struct message));
	m->msg_len = len-2;
	switch (msg[0]) {
		case '0':
			m->nr = ACK_NACK;
			if(m->msg_len>0){
				m->msg = malloc((len-1) * sizeof(char));
				strncpy(m->msg, msg+2, len-2);
				m->msg[len-2] = 0;
			}else m->msg = 0;
			break;
		case '1':
			m->nr = IF_LIST;
			m->msg = malloc((len-1) * sizeof(char));
			strncpy(m->msg, msg+2, len-2);
			m->msg[len-2] = 0;
			break;
		case '2':
			m->nr = DEV_INFO;
			m->msg = malloc((len-1) * sizeof(char));
			strncpy(m->msg, msg+2, len-2);
			m->msg[len-2] = 0;
			break;
		case '3':
			m->nr = SET_PORT;
			m->msg = malloc((len-1) * sizeof(char));
			strncpy(m->msg, msg+2, len-2);
			m->msg[len-2] = 0;
			break;
		case '4':
			m->nr = SET_MAC;
			m->msg = malloc((len-1) * sizeof(char));
			strncpy(m->msg, msg+2, len-2);
			m->msg[len-2] = 0;
			break;
		default:
			free(m);
			m = 0;
	}

	free(msg);
	return m;
}

void delete_message(struct message* m)
{
	if (m)
		if (m->msg)
			free(m->msg);
		free(m);
}


