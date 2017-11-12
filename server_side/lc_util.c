#include "lc_util.h"
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <wait.h>
#include <netdb.h>
#include <stdbool.h>
#include "lc_msg_api.h"
// #define DEBUG

#define MAXSLEEP 8
#define MAX_NODE ((10))
const int port_list[MAX_NODE+1]={
    8080,8081,8082,8083,8084,
    8085,8086,8087,8088,8089,8090
};


int lc_node_sum = 10;
int lc_node_num = 0;
int lc_logic_clock = 0;
bool is_clock_sync = false;

// used to sync node's logic clock
int lc_node_clock_sum = 0;
int lc_node_clock_count = 0;

int rand_in_range(int minimum_number, int max_number){
  srand(getpid());
  return rand() % ((max_number + 1 - minimum_number) + minimum_number);
}

void lc_init_clock(void) {
  lc_logic_clock = rand_in_range(1,20);
  // #ifdef DEBUG
  printf("Node %d(PID:%d) has clock:%d\n", lc_node_num, getpid(), lc_logic_clock );
  // #endif
}
void lc_sync_clock(void) {
  struct lc_msg msg;
  msg.msg_type_ = CMD;
  msg.pid_ = getpid();
  msg.time_ = lc_logic_clock;
  sprintf(msg.msg_, "%s","SYNC_CLOCK");
  // send sync cmd to all nodes
  lc_broadcast_msg(msg);
}

// breadcast msg to all node except parent process
void lc_broadcast_msg(struct lc_msg msg) {
  int sockfd = 0, n = 0;
  char sendBuff[64];
  struct sockaddr_in serv_addr;
  memset(sendBuff, 0,sizeof(sendBuff));
  encode_msg(sendBuff,msg);
  if(msg.msg_type_ == MSG){
    // lamport's clock rule, before sending a message, logic clock plus 1
    lc_logic_clock++;
    printf("Node %d bcst(PID:%d T:%d):%s\n",lc_node_num,getpid(), lc_logic_clock, sendBuff );
  }
  for (int i = 0; i < lc_node_sum; ++i) {
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  	{
  		printf("\n Error : Could not create socket \n");
  		return ;
  	}
    memset(&serv_addr, 0, sizeof(serv_addr));

  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_port = htons(port_list[i]);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
  	{
  		printf("\n inet_pton error occured\n");
  		return;
  	}

    // try to build a connection
    for(int nsec = 1; nsec <= MAXSLEEP; nsec <<= 1) {
     		if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
        // Connection accepted
          #ifdef DEBUG
            printf("\t Connection Success : Node %d to Node %d. Sockid %d \n",lc_node_num,i,sockfd);
          #endif
    			break;
        }
    		else {
    			if (nsec <= MAXSLEEP/2) {
    				sleep(nsec);
    			}
    			else {
    				printf("\n Error : Node %d connect Node %d Failed \n",lc_node_num,i);
            exit(0);
    				return;
    			}
    		}
      } // end of try connection


    // sprintf(sendBuff, "Node %d send msg to Node %d", lc_node_num, i );
    send(sockfd, sendBuff, sizeof(sendBuff), MSG_DONTWAIT);
    shutdown(sockfd, SHUT_WR);  //msg 发送完毕，断开输出流，向对方发送FIN包
    // shutdown(sockfd,SHUT_WR);
  }// finish broadcast

}
void lc_send_msg(int port, struct lc_msg msg) {
  int sockfd = 0, n = 0;
  char sendBuff[64];
  struct sockaddr_in serv_addr;
  memset(sendBuff, 0,sizeof(sendBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Error : Could not create socket \n");
    return ;
  }
  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
  {
    printf("\n inet_pton error occured\n");
    return;
  }

  for(int nsec = 1; nsec <= MAXSLEEP; nsec <<= 1) {
      if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
      // Connection accepted
        #ifdef DEBUG
          printf("\t Connect port %d success\n",port);
        #endif
        break;
      }
      else {
        if (nsec <= MAXSLEEP/2) {
          sleep(nsec);
        }
        else {
          printf("\n Error : Connect port %d Failed \n",port);
          exit(0);
          return;
        }
      }
    } // end of for

  encode_msg(sendBuff,msg);
  // sprintf(sendBuff, "Node %d send msg to Node %d", lc_node_num, i );
  send(sockfd, sendBuff, sizeof(sendBuff), MSG_DONTWAIT);
  shutdown(sockfd, SHUT_WR);  //msg 发送完毕，断开输出流，向对方发送FIN包
}

typedef void (*lc_deal_func)(struct lc_msg );
void lc_node_deal_with_msg(struct lc_msg msg) {
  char buffer[64];
  encode_msg(buffer, msg);

  switch(msg.msg_type_){
    case CMD: // used to sync clock
      // send node clock to parent
      if(strncmp(msg.msg_, "SYNC", 4) == 0) {
          struct lc_msg m;
          m.time_ = lc_logic_clock;
          m.msg_type_ = CMD;
          m.pid_ = getpid();
          sprintf(m.msg_,"%s","SYNC");
          lc_send_msg(port_list[lc_node_sum],m);
      }
      else if (strncmp(msg.msg_, "SET_CLOCK",9) == 0) {
        lc_logic_clock = msg.time_;
        is_clock_sync = true;
        // printf("Node %d set clock:%s\n", lc_node_num, buffer);
      }

    break;

    case MSG:
      // lamport's clock rule, after recv a msg, it adjust it's logic clock to the max value between time stamp and its own clock
      lc_logic_clock = lc_logic_clock > msg.time_ ? lc_logic_clock: msg.time_;
      lc_logic_clock++;
      printf("Node %d recv(PID:%d T:%d):%s\n",lc_node_num,getpid(), lc_logic_clock, buffer );
    break;

    default:
    break;
  }
}

void lc_parent_deal_with_msg(struct lc_msg msg) {
  char buffer[64];
  encode_msg(buffer, msg);
  // printf("Parent %d(PID:%d) recv:%s\n",lc_node_num,getpid(), buffer );
  switch(msg.msg_type_) {
    case CMD:
        if(strncmp(msg.msg_, "SYNC", 4) == 0) {
          lc_node_clock_count++;
          lc_node_clock_sum += msg.time_;

          if(lc_node_clock_count == lc_node_sum) {// all node have send parent its clock value
            int lc_standard_clock = lc_node_clock_sum / lc_node_clock_count;
            struct lc_msg m = msg_generate(CMD, lc_standard_clock, getpid(), "SET_CLOCK");
            lc_broadcast_msg(m);
          }
        }
    break;

    case MSG:
    break;

    default:
    break;
  }
}


// message recieve service
// define different msg recieve func, and as a parameter to pass to the service
void* lc_recv_service(void* func) {
      fd_set master;    // master file descriptor list
      fd_set read_fds;  // temp file descriptor list for select()
      int fdmax;        // maximum file descriptor number

      int listener;     // listening socket descriptor
      int newfd;        // newly accept()ed socket descriptor
      struct sockaddr_storage remoteaddr; // client address
      socklen_t addrlen;

      char buf[256];    // buffer for client data
      int nbytes;

      char remoteIP[INET6_ADDRSTRLEN];

      int yes=1;        // for setsockopt() SO_REUSEADDR, below
      int i, j, rv;

      struct addrinfo hints, *ai, *p;
      char port[64];
      sprintf(port,"%d",port_list[lc_node_num]);

      FD_ZERO(&master);    // clear the master and temp sets
      FD_ZERO(&read_fds);

      // get us a socket and bind it
      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE;
      if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
          fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
          exit(1);
      }

      for(p = ai; p != NULL; p = p->ai_next) {
          listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
          if (listener < 0) {
              continue;
          }

          // lose the pesky "address already in use" error message
          setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

          if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
              close(listener);
              continue;
          }

          break;
      }

      // if we got here, it means we didn't get bound
      if (p == NULL) {
          fprintf(stderr, "selectserver: failed to bind\n");
          exit(2);
      }

      freeaddrinfo(ai); // all done with this

      // listen
      if (listen(listener, 10) == -1) {
          perror("listen");
          exit(3);
      }

      // add the listener to the master set
      FD_SET(listener, &master);

      // keep track of the biggest file descriptor
      fdmax = listener; // so far, it's this one

      // main loop
      for(;;) {
          read_fds = master; // copy it
          if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
              perror("select");
              exit(4);
          }

          // run through the existing connections looking for data to read
          for(i = 0; i <= fdmax; i++) {
              if (FD_ISSET(i, &read_fds)) { // we got one!!
                  if (i == listener) {
                      // handle new connections
                      addrlen = sizeof remoteaddr;
                      newfd = accept(listener,
                          (struct sockaddr *)&remoteaddr,
                          &addrlen);

                      if (newfd == -1) {
                          perror("accept");
                      } else {
                          FD_SET(newfd, &master); // add to master set
                          if (newfd > fdmax) {    // keep track of the max
                              fdmax = newfd;
                          }
                      }
                  } else {
                      // handle data from a client
                      if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                          // got error or connection closed by client
                          if (nbytes == 0) {
                              // connection closed
                              #ifdef DEBUG
                              printf("selectserver: socket %d hung up\n", i);
                              #endif
                          } else {
                              perror("recv");
                          }
                          close(i); // bye!
                          FD_CLR(i, &master); // remove from master set
                      } else {
                          // we got some data from a client
                          struct lc_msg m;
                          decode_msg(buf,&m);
                          ((lc_deal_func)(func))(m);

                          // for(j = 0; j <= fdmax; j++) {
                          //     // send to everyone!
                          //     if (FD_ISSET(j, &master)) {
                          //         // except the listener and ourselves
                          //         if (j != listener && j != i) {
                          //             if (send(j, buf, nbytes, 0) == -1) {
                          //                 perror("send");
                          //             }
                          //         }
                          //     }
                          // }
                      }
                  } // END handle data from client
              } // END got new incoming connection
          } // END looping through file descriptors
      } // END for(;;)--and you thought it would never end!
}

void pr_exit(int status)
{
  if (WIFEXITED(status))
  printf("normal termination, exit status = %d\n",  WEXITSTATUS(status));
  else if (WIFSIGNALED(status))
    printf("abnormal termination, signal number = %d%s\n",  WTERMSIG(status),
  #ifdef WCOREDUMP
  WCOREDUMP(status) ? " (core file generated)" : "");
  #else
  "");
  #endif
  else if (WIFSTOPPED(status))
  printf("child stopped, signal number = %d\n",  WSTOPSIG(status));
}
