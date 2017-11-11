#include "mylib.h"
#include "lc_util.h"
#include "lc_msg_api.h"

int main(int argc, char *argv[])
{
    // record parent process pid
    pid_t parent_pid = getpid();
    // set number of nodes
    if(argc == 2) {
        lc_node_sum = atoi(argv[1]);
        lc_node_sum = lc_node_sum > 10? 10: lc_node_sum;
    }
    printf("There are %d nodes\r\n", lc_node_sum);
    fflush(stdout);
    pid_t wpid,pid;
    int status = 0;

    // create lc_node_sum processes, each process represent a node
    for (int i = 0; i < lc_node_sum; i++) {
        if ((pid = fork()) < 0) {
            printf("fork error");
            break;
        }else if (pid == 0) { // child process
            // record current node's number
            lc_node_num = i;

            lc_init_clock();
            // create a service thread to recieve message
            pthread_t precv_msg;
            pthread_create(&precv_msg, NULL, lc_recv_service, NULL);

            break;
        }else { // parent process

        }
    }

    // this is main thread

    // child prcocess send message to each other
    if(getpid() != parent_pid) {
      // while(!is_clock_sync); // wait until clock is synchronized
      struct lc_msg msg;
      msg.msg_type_ = CMD;
      msg.pid_ = getpid();
      msg.time_ = lc_logic_clock;
      sprintf(msg.msg_, "%s","Test");
      lc_broadcast_msg(msg);
    }
    else { //parent process as the time deamon
      lc_sync_clock();
    }

    // wait for all child process end
    while ((wpid=waitpid(-1,&status,0))!=-1) {
      printf("Process %d terminated\n",wpid);
      pr_exit(status);
    }

    pthread_exit(NULL);
}
