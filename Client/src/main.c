#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../../src/protocol.h"

#define MAXLINE 4096
#define PORT 3000
#define NUM_THREADS 2
void *Datareader(void *fde)
{
    int fd;
    fd=(int)fde;
    struct message *receive;
    while(1){
    receive=receive_message(fd);
            if(receive!=0)
            switch(receive->nr){
                case 0:
                    puts(receive->msg);
                    break;
                default:
                    printf("Błąd podczas wykonywania polecenia\n");
            }
            delete_message(receive);
    }
   pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    pthread_t threads[NUM_THREADS];
    int rc;
    long t=0;
    char *sendline=malloc(sizeof(char)*MAXLINE);

    if((sockfd = socket(AF_INET, SOCK_STREAM,0))<0){
        perror("PROBLEM in creating the socket");
        return 2;
    }
    memset(&servaddr, 0, sizeof(servaddr));

    rc = pthread_create(&threads[t], NULL, Datareader, (void*)sockfd);
    if (rc){
     printf("ERROR; return code from pthread_create() is %d\n", rc);
     exit(-1);
    }

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
        }
    }
    free(sendline);
    pthread_exit(NULL);
    free(sendmsg);
    free(tmp_message);
    free(message_type);
    close(sockfd);
    return 0;
}