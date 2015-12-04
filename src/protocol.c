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
						{ SET_PORT, "set_ip" }
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
int n_keys=sizeof(message_key) / sizeof(message_key[0]);
int i;
for (i = 0; i < n_keys; i++)
	if (strcmp(str, message_key[i].str) == 0)
		return message_key[i].id;

return ACK_NACK;	//lub najlepiej jakiś error
}

int send_ack_nack(int fd, bool is_error, const char* error_msg)
{
	char* msg = "0;0";
	size_t len = 1;
	if (is_error) {
		len = strlen(error_msg) + 4;
		msg = malloc((len + 1) * sizeof(char));
		strcpy(msg, "0;1;");
		strcpy(msg+4, error_msg);
	}

	return send_bytes(fd, msg, len);
}

int send_message_to_server(int fd,const char* msg,size_t len)
{

	send_bytes(fd,msg,len);
	return 0;
}

int send_dev_info(int fd)
{

	return 0;
}

/*int send_user_list_reply(int fd, const char* names[], size_t len)
{
	char* msg = 0;
	size_t mlen = 2;
	size_t i = 0;
	size_t offset = 0;
	for (; i < len; ++i) {
		mlen += strlen(names[i]) + 1; //dot separator
	}
	msg = malloc(mlen * sizeof(char));
	strcpy(msg, "7.");
	offset = 2;
	for (i = 0; i < len; ++i) {
		strcpy(msg+offset, names[i]);
		offset += strlen(names[i]);
		if (i < (len-1)) {
			msg[offset] = '.';
			++offset;
		}
	}

	return send_bytes(fd, msg, mlen);
}*/

struct message* receive_message(int fd)
{
	size_t len = 0;
	char* msg = 0;
	struct message* m = 0;

	if (read(fd, &len, sizeof(size_t)) < 1)
		return 0;

	msg = malloc(len * sizeof(char));
	if (read(fd, msg, len) != len) {
		return 0;
	}

	m = malloc(sizeof(struct message));
	m->msg_len=len;
	switch (msg[0]) {
		case '0':
			m->nr = ACK_NACK;
			m->msg = 0;
			break;
		case '1':
			m->nr = IF_LIST;
			m->msg_len = strlen(msg)-2;
			m->msg = malloc((len-1) * sizeof(char));
			strncpy(m->msg, msg+2, len-2);
			m->msg[len-2] = 0;
			break;
		case '2':
			m->nr = DEV_INFO;
			m->msg_len = strlen(msg)-2;
			m->msg = malloc((len-1) * sizeof(char));
			strncpy(m->msg, msg+2, len-2);
			m->msg[len-2] = 0;
			break;
		case '3':
			m->nr = SET_PORT;
			m->msg_len = strlen(msg)-2;
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
	if (m->msg)
		free(m->msg);
	free(m);
}
void sp_to_(char *str)
{
  while(*str) {
    if(*str== ' ') 
        *str=';';
    else
    	str++;
  }
}

