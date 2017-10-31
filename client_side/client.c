#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "server_util.h"

#define MAXSLEEP 128
FileServerUtil su;
int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0;
	char recvBuff[256], sendBuff[256];
	struct sockaddr_in serv_addr; 
  int nsec;
	if(argc != 3)
	{
		printf("\n Usage: %s <ip of server> message \n",argv[0]);
		return 1;
	} 

	memset(recvBuff, 0,sizeof(recvBuff));
	memset(sendBuff, 0,sizeof(recvBuff));
        strcpy(sendBuff, argv[2]);
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	} 

	memset(&serv_addr, 0, sizeof(serv_addr)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080); 

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}
  for(nsec = 1; nsec <= MAXSLEEP; nsec <<= 1) { 
		
 		if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
    // Connection accepted 
			break;
    }
		else {
			if (nsec <= MAXSLEEP/2) {
				sleep(nsec);
			}
			else {
				printf("\n Error : Connect Failed \n");
				return 1;
			}
		}
  }
  // send command to server
  send(sockfd, sendBuff, sizeof(sendBuff), MSG_DONTWAIT);
	ClientMsgType msg_type = su.MsgType(sendBuff);
	recv(sockfd, recvBuff, sizeof(recvBuff), MSG_WAITALL);	
  printf("Message from the server:%s\r\n", recvBuff);
	if (strncmp("Server ACK", recvBuff, 10) != 0) {
		return 0;
	}
	memset(sendBuff, 0, sizeof(sendBuff));
	memset(recvBuff, 0, sizeof(recvBuff));
  int fd;
  uint64_t n_count = 0;
	uint64_t offset = 0;
  switch(msg_type) {
		case kUpload:
			// Input filename then send
			printf("Input name of the file you want to upload:");
			scanf("%s", sendBuff);
			send(sockfd, sendBuff, sizeof(sendBuff), MSG_DONTWAIT);
			fd = open(sendBuff,O_RDONLY, 0666);
			memset(sendBuff, 0, sizeof(sendBuff));
			if(fd > 0) {
     		while( (n_count = pread(fd, sendBuff, sizeof(sendBuff), offset)) > 0) {
					offset += n_count;
					send(sockfd, sendBuff, n_count, MSG_DONTWAIT);
     		}
			}
			shutdown(sockfd, SHUT_WR);  //文件读取完毕，断开输出流，向客户端发送FIN包
     	printf("File uploaded\r\n");
   	break;

   	case kDelete:
		printf("Input name of the file you want to delete:");
		scanf("%s", sendBuff);
		send(sockfd, sendBuff, sizeof(sendBuff), MSG_DONTWAIT);
		shutdown(sockfd, SHUT_WR);
   	break;

   	case kRename:
		printf("Input old & new name\' format:oldname-newname\':");
		scanf("%s", sendBuff);
		send(sockfd, sendBuff, sizeof(sendBuff), MSG_DONTWAIT);
		shutdown(sockfd, SHUT_WR);
   	break;

   	case kDownld:
		printf("Input name of the file you want to download:");
		scanf("%s",sendBuff);
		send(sockfd, sendBuff, sizeof(sendBuff), MSG_DONTWAIT);
    fd = open(sendBuff, O_CREAT | O_RDWR | O_TRUNC, 0666);
    memset(sendBuff, 0, sizeof(sendBuff)); 
    while( (n_count = recv(sockfd, recvBuff, sizeof(recvBuff), MSG_WAITALL)) > 0) {
      write(fd, recvBuff, n_count);
    }
    close(fd);
    printf("File download\r\n");
		shutdown(sockfd, SHUT_WR);
   	break;

   	case kError:
		printf("Wrong parameters");
		shutdown(sockfd, SHUT_WR);
   	break;
  }

	return 0;
}
