#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "protocol.h"

#define MAXLINE 4096
#define SERV_PORT 3000

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];//, recvline[MAXLINE];

    if((sockfd = socket(AF_INET, SOCK_STREAM,0))<0){
        perror("PROBLEM in creating the socket");
        return 2;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//argv[1]
    servaddr.sin_port = htons(SERV_PORT);

    if(connect(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0){
        perror("Problem in create connect");
        return 3;
    }
    //struct message m=0;
    char *sendmsg=0;
    char pch[MAXLINE];
    int nw=0;
    while(1){
        scanf("%s",sendline);
        strcpy(pch,sendline);
        strtok(sendline," ");
        switch (parse_message_key(sendline)){
            case 0:                                      //ack_nack
                printf("ACK_NACK\n" );
                break;
            case 1:                                      //lista interface
                printf("if_list\n" );
                //m->nr=IF_LIST;
                sp_to_(pch);
                printf("%s\n",pch );
                
                break;
            case 2:                                      //informacje
                printf("devinfo\n" );
                break;
            case 3:                                      //setport
                printf("Default\n" );
                break;
            default:
                printf("Brak takiego polecenia\n");
                break;
        }
        //strcat(sendline,";");
        //printf("%s", sendline);
        
        //printf("Send msg %s\n", sendmsg);
        write(sockfd,sendline,strlen(sendline));
        //m=receive_message(sockfd);

        //printf("%s", "String odczytany z serwera: ");
        //fputs(m->msg,stdout);
    }

    return 0;

}




    
