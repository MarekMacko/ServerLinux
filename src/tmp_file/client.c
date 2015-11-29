#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>

void error(const char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char **argv)
{
	int sock_fd, port_no, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if(argc < 3){
		error("Not enaugh arguments!");
	}

	port_no = atoi(argv[2]);
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0)
		error("socket");

	server = gethostbyname(argv[1]);
	if(server == NULL)
		error("server get host");

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
//	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port_no);
	if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		error("connect");
	}
	printf("Enter message:\n");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n = write(sock_fd, buffer, strlen(buffer));
	if(n < 0)
		error("write");
	
	bzero(buffer,256);
	n = read(sock_fd, buffer, 255);
	if(n < 0)
		error("read");
	printf("Message from server:%s\n", buffer);
	close(sock_fd);

	return 0;
}
