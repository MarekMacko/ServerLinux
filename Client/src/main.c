#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "../../src/protocol.h"

#define MAXLINE 4096
#define PORT 3000

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char *sendline=malloc(sizeof(char)*MAXLINE);

    if((sockfd = socket(AF_INET, SOCK_STREAM,0))<0){
        perror("PROBLEM in creating the socket");
        return 2;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    if(argc>1)
        {
        strcpy(sendline,argv[1]);
        strtok(argv[1],".");
        if(strtok(0,".")!=NULL){
            servaddr.sin_addr.s_addr = inet_addr(sendline);
            if(argc==3){
                servaddr.sin_port = htons(atoi(argv[2]));
            }else{
                servaddr.sin_port = htons(PORT);
            }
        }else
        {
            servaddr.sin_port = htons(atoi(argv[1]));
            servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        }
    }else{
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }

    //memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//argv[1]
    

    if(connect(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr))<0){
        perror("Problem in create connect");
        return 3;
    }

    char *tmp_sendmsg=sendline;     //potrzebny wskaznik na początek sendline
    char *sendmsg=malloc(sizeof(char)*MAXLINE);     //dokładna wiadomość dodaje nr uslugi
    char *tmp_message=malloc(sizeof(char)*MAXLINE);     //wiadomość przygotowywana
    char *message_type=malloc(sizeof(char)*20); //typ wiadomości
    struct message *msg;
    //char pch[MAXLINE];
    //int nw=0;
    while(1){
        sendline=tmp_sendmsg;
        memset(sendline,0,sizeof(*sendline)*MAXLINE);//czyszczenie buforu
        fgets(sendline,MAXLINE,stdin);
        sendline=strtok(sendline," ");

        //if długość polecenia jest mniejsza niż 20 znaków
        if(strlen(sendline)<20){
            memset(message_type,0,sizeof(char)*20);
            strcpy(message_type,sendline);
        }else{
            message_type=0;
        }

        //prepare message
        memset(tmp_message,0,sizeof(char)*20);
        sendline=strtok(0," ");
        while(sendline != 0){
            strcat(tmp_message,sendline);
            strcat(tmp_message,";");
            sendline=strtok(0," ");
        }
        //end prepare message
        sendline=tmp_sendmsg;
        memset(sendline,0,sizeof(char)*MAXLINE);
        memset(sendmsg,0,sizeof(char)*MAXLINE);

        switch (parse_message_key(message_type)){
            case IF_LIST:                                      //lista interface
                strcat(sendmsg,"1;");
                strcat(sendmsg, tmp_message);
                strncpy(sendline,sendmsg,strlen(sendmsg)-2);    //usuwa dwa ostatnie znaki czyli "/n;"
                break;
            case DEV_INFO:                                      //informacje
                strcat(sendmsg,"2;");
                strcat(sendmsg, tmp_message);
                strncpy(sendline,sendmsg,strlen(sendmsg)-2);
                break;
            case SET_PORT:   
                strcat(sendmsg,"3;");
                strcat(sendmsg, tmp_message);
                strncpy(sendline,sendmsg,strlen(sendmsg)-2);
                break;
            case SET_MAC:   
                strcat(sendmsg,"4;");
                strcat(sendmsg, tmp_message);
                strncpy(sendline,sendmsg,strlen(sendmsg)-2);
                break;
            default:
                printf("Brak takiego polecenia\n");
                sendline=0;
                break;
        }
        if(sendline!=0){
            send_message_to_server(sockfd,sendline,strlen(sendline));
            msg=receive_message(sockfd);

        
            switch(msg->nr){
                case 0:
                    printf("%s\n",msg->msg);
                    break;
                default:
                    printf("Błąd podczas wykonywania polecenia\n");
            }
            delete_message(msg);
        }
       
        
    }
    free(sendline);
    free(sendmsg);
    free(tmp_message);
    free(message_type);
    close(sockfd);
    return 0;

}