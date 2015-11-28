#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libconfig.h>

#define LISTEN_BACKLOG 50

typedef struct server_settings{
	int port;
	int max_clients;
}server_sett;

void dostuff(int);
void error(const char *msg);
void read_settings(const char *, struct server_settings *);

int main(int argc, char **argv)
{
	const char* const socket_name = argv[1];
	int sock_fd, cli_sock_fd, port_no;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_addr_len;
	struct server_settings serv_sett;
	pid_t pid;
		
	read_settings("config.cfg", &serv_sett);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1)
 		error("socket");
 	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(serv_sett.port);
	
	if(bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		error("bind");
	
	if(listen(sock_fd, LISTEN_BACKLOG) == -1)
		error("listen");
	
	cli_addr_len = sizeof(struct sockaddr_un);

	while(1){
		printf("wait for accept\n");
		cli_sock_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &cli_addr_len);
		if(cli_sock_fd < 0) 
			error("accept");
 		pid = fork();
		if(pid < 0)
			error("pid");
		if(pid == 0){ //child process
			close(sock_fd);
			dostuff(cli_sock_fd);
			exit(0);
		}
		else
 			close(cli_sock_fd);
	}
	close(sock_fd);
	return 0;
}

void dostuff(int sock){
	int n;
	char buffer[256];

	bzero(buffer, 256);
	n = read(sock, buffer, 255);
	if(n < 0) 
		perror("read");
	printf("Message: %s\n", buffer);
	n = write(sock,"I got your message",18);
	if(n < 0)
		perror("write");
}

void error(const char *msg){
	perror(msg);
	exit(1);
}

void read_settings(const char *config_file_name, struct server_settings *serv_sett){
	config_t cfg;
	config_setting_t *settings;

	/*Initialization*/
	config_init(&cfg);

	/*Read the file. If there is an error, report it and exit*/
	if(!config_read_file(&cfg, config_file_name)){
		printf("%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		error("Read settings error\n");
	} else {
		printf("Read settings successfully\n");
	}
	
	if(config_lookup_int(&cfg, "port", &serv_sett->port)){
		printf("Port = %d\n", serv_sett->port);
	} else {
		printf("No 'port' setttings in configuration file\n");
	}
	
	if(config_lookup_int(&cfg, "max_clients", &serv_sett->max_clients)){
		printf("Max clients = %d\n", serv_sett->max_clients);
	} else {
		printf("No 'max clients' setttings in configuration file\n");
	}
}

