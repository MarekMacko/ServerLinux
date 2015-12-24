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

int sockfd=-1;
int loop=1;

void *Datareader(void *fde);


int main(int argc, char **argv)
{
    //int sockfd;
    struct sockaddr_in servaddr;
    pthread_t thread;
    int rc;
    //long t=0;
    char *sendline=malloc(sizeof(char)*MAXLINE);

    if((sockfd = socket(AF_INET, SOCK_STREAM,0))<0){
        perror("Socket: ");
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));

    rc = pthread_create(&thread, NULL, Datareader, NULL);
    if (rc){
     perror("Pthread: ");
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
        perror("Connect: ");
        return -1;
    }

    char *tmp_sendmsg=sendline;     //potrzebny wskaznik na początek sendline
    char *sendmsg=malloc(sizeof(char)*MAXLINE);     //dokładna wiadomość dodaje nr uslugi
    char *tmp_message=malloc(sizeof(char)*MAXLINE);     //wiadomość przygotowywana
    char *message_type=malloc(sizeof(char)*20); //typ wiadomości
    //char pch[MAXLINE];
    int nw=0;
    while(loop){
        sendline=tmp_sendmsg;
        memset(sendline,0,sizeof(*sendline)*MAXLINE);//czyszczenie buforu
        fgets(sendline,MAXLINE-1,stdin);
        sendline=strtok(sendline," ");

        nw=strlen(sendline);
        //if długość polecenia jest mniejsza niż 20 znaków
        if(nw<20){
            memset(message_type,0,sizeof(char)*20);
            strcpy(message_type,sendline);
        }else{
            *message_type=0;
        }

        if(message_type!=0){
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
        }
        switch (parse_message_key(message_type)){
            case IF_LIST:                                      //lista interface
                strcat(sendmsg,"1;");
                //strcat(sendmsg, tmp_message);
                strncpy(sendline,sendmsg,strlen(sendmsg));    //usuwa dwa ostatnie znaki czyli "/n;"
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
            case EXIT:
                loop=0;
                break;
            case HELP:
                //tutaj jakiś display helpa można zrobić
                break;
            default:
                printf("Brak takiego polecenia\n");
                sendline=0;
                break;
        }
        
        //printf("%s\n",);
        if(sendline){
            send_message_to_server(sockfd,sendline,strlen(sendline)); 
        }
    }
    //free(tmp_message);
    pthread_join(thread,NULL);
    free(sendline);
    //(void) pthread_join(rc,NULL);
    free(sendmsg);
    free(message_type);
    free(tmp_message);
    close(sockfd);
    //pthread_cancel(rc);
    return 0;
}

void *Datareader(void *fde)
{
    //int fd=(int)fde;
    struct message *receive;
    while(loop){
    //printf("%d\n",sockfd);
    receive=receive_message(sockfd);
            if(receive!=0){
                switch(receive->nr){
                    case 0:
                        puts(receive->msg);
                        break;
                    default:
                        printf("Błąd podczas wykonywania polecenia\n");
                }
            }else{
            loop=0;
            delete_message(receive);
            printf("Klient został rozłączony\n");
        }
    }
    pthread_exit(NULL);
}