#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include "server_util.h"

#define R_B_SIZE (sizeof(recvBuff))
#define S_B_SIZE (sizeof(sendBuff))

const int port = 8080;
FileServerUtil su;
int eof;

void *DealWithData(void* connfd);

int main(int argc, char *argv[])
{
    int listenfd = 0;
    struct sockaddr_in serv_addr;
    char abuf[INET_ADDRSTRLEN];
    const char* addr;
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 50);

    addr = inet_ntop(AF_INET, &serv_addr.sin_addr, abuf, INET_ADDRSTRLEN);
    printf("Server Address %s:%d\r\n", abuf, port);
    while(1)
    {
        int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        pthread_t thread;
        pthread_create(&thread, NULL, DealWithData,(&connfd));
    }
}


void *DealWithData(void* p_connfd) {
    uint64_t tid = pthread_self();
    int connfd =*( (int*)p_connfd);
    char sendBuff[256], recvBuff[256];
    recv(connfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
    printf("Message from client: %s - %lu\r\n", recvBuff, tid);
    // Deal with message type
    ClientMsgType msg_type = su.MsgType(recvBuff);
    printf("Message is %d - %lu\r\n", msg_type, tid);
    strcpy(sendBuff, "Server ACK\n");
    send(connfd, sendBuff, sizeof(recvBuff), MSG_DONTWAIT);
    memset(recvBuff, 0, sizeof(recvBuff));
    memset(sendBuff, 0, S_B_SIZE);
    int fd;
    int n_count;
    int result = -1;
    int offset = 0;
    switch(msg_type) {
    case kUpload:
      n_count = recv(connfd, recvBuff, R_B_SIZE, MSG_WAITALL);
      if (n_count == 0) {
        printf("connection end - %lu\r\n", tid);
                break;
            }
            fd = open(recvBuff, O_CREAT | O_RDWR | O_TRUNC, 0666);
            memset(recvBuff, 0, R_B_SIZE);
            while( (n_count = recv(connfd, recvBuff, R_B_SIZE, MSG_WAITALL)) > 0) {
                write(fd, recvBuff, n_count);
            }
            close(fd);
            printf("File uploaded - %lu\r\n", tid);
        break;

    case kDelete:
        recv(connfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
        result = su.Delete(recvBuff);
        if (result == 0) 
            printf("file deleted - %lu\r\n", tid);
        else
            printf("Delete operation fail. Code:%d - %lu\r\n", result, tid);
        send(connfd, "file deleted\r\n", 14, MSG_DONTWAIT);
    break;

    case kRename:
        char* old_name,* new_name;
        recv(connfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
        old_name = strtok(recvBuff,"-");
        new_name = strtok(NULL,"-");
        if (new_name != NULL)
            result = su.ReName(old_name, new_name);
        printf("Rename result%d - %lu\r\n", result, tid);
    break;

    case kDownld:
        char* filename;
        recv(connfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);
        fd = open(recvBuff, O_RDONLY, 0666);
        memset(sendBuff, 0, sizeof(sendBuff));
        if (fd > 0) {
            while ( (n_count = pread(fd, sendBuff, sizeof(sendBuff), offset)) > 0) {
                offset += n_count;
                send(connfd, sendBuff, n_count, MSG_DONTWAIT);
            }
        }
        shutdown(connfd, SHUT_WR);
    break;

    case kError:
    printf("Error msg - %lu\r\n", tid);
    break;
    }// end of switch
    // session ends
    close(connfd);
    sleep(1);
}

