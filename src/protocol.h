#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <stdbool.h>

enum message_type {
	ACK_NACK,	//wiadomość potwierdzająca
	IF_LIST,	//pobieranie listy interface
	DEV_INFO,	//pobieranie ustawien interface
	SET_PORT,	//ustawianie ip maski i macu na porcie
	SET_MAC
};



struct message {
	enum message_type nr;
	size_t msg_len;	//to za bardzo nie powinno być potrzebne, bo funkcja send sobie to oblicza
	char* msg;
};

int send_ack_nack(int fd, bool is_error, const char* error_msg);
int send_message_to_server(int fd,const char* msg,size_t len);
int send_dev_info(int fd);
struct message* receive_message(int fd);
void delete_message(struct message* m);
int parse_message_key(const char *str);

#endif