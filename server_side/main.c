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
    printf("There are %d nodes, parent pid %d\r\n", lc_node_sum, getpid());
    fflush(stdout);
    pid_t wpid,pid;
    int status = 0;

    queue_init(&lc_order_msg_queue, TOTAL);
    queue_init(&lc_order_issue_queue, TOTAL);
    queue_init(&lc_FIFO_queue, PARTIAL);

    initial_file(lc_node_sum);
    sleep(2);
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
            pthread_create(&precv_msg, NULL, lc_recv_service, (void*)(lc_node_deal_with_msg));

            break;
        }else { // parent process

        }
    }

    // this is main thread

    // child prcocess send message to each other
    if(getpid() != parent_pid) {
      while(!is_clock_sync); // wait until clock is synchronized
      printf("Node %d clock synchronized. clock is: %d\n", lc_node_num, lc_logic_clock);
      sleep(1);// wait for all node synchronize

      // after clock is synchronized, then node isdue an event
      char buffer[64];
      sprintf(buffer, "ISSUE_FROM_NODE%d", lc_node_num);
      // lc_issue_count++; // issue count plus one

      // current node broadcast the issue m
      lc_broadcast_msg(msg_generate(MSG,lc_logic_clock,getpid(),buffer));

      while(1) {
        // Lamport's totally order multicasting
        // if(lc_order_msg_queue.size_ != 0)
        { // There exist msg have not beed ACK
          // following is three condition to send ACK, and two condition combine to one in the first one
          // this is the condition that 1.Pi has not made an update request or 2.Pi's update has been processed
          if(lc_issue_count == 0) { // there is no new issue, then broadcast ACK for all msg in queue
            while(!lc_order_msg_queue.isEmpty(&lc_order_msg_queue)) {
              lc_msg m = lc_order_msg_queue.dequeue(&lc_order_msg_queue);
              sprintf(m.msg_,"ACK_%d_%d",m.time_,m.pid_);
              lc_broadcast_msg(msg_generate(MSG,lc_logic_clock,getpid(),m.msg_)); // ACK for this particular message
            }
          }
          else { // Here we send ACK for Pi's identifier is greater than or equal to Pj's identifier
            while(!lc_order_msg_queue.isEmpty(&lc_order_msg_queue)) {
              // printf("queue size:%d\n",lc_order_queue.size_ );
              lc_msg m = lc_order_msg_queue.dequeue(&lc_order_msg_queue);
              // printf("queue size after de:%d\n",lc_order_queue.size_ );
              sprintf(m.msg_,"ACK_%d_%d",m.time_,m.pid_);
              lc_broadcast_msg(msg_generate(MSG,lc_logic_clock,getpid(),m.msg_)); // ACK for this particular message

              if ( getpid() == m.pid_){
                break;
              }
            }
          }
        }// lc_order_msg_queue is empty now
        // end of Lamport's totally order multicasting

        // process update once it is ack by all nodes
        if (!lc_order_issue_queue.isEmpty(&lc_order_issue_queue))
        {
          lc_msg front_issue = lc_order_issue_queue.peek(&lc_order_issue_queue);
          if(front_issue.ack_time_ == lc_node_sum) { // the head element of the queue has been ack by all nodes, can be processed
            lc_msg issue_event = lc_order_issue_queue.dequeue(&lc_order_issue_queue);
            if (issue_event.pid_ == getpid()) { // 如果是自己的事件,那么 lc_issue_count --
              lc_issue_count--;
            }
            char buf[64];
            printf("\t Total Order: Node %d issue <time:%d, pid:%d, msg:%s>\n", lc_node_num, issue_event.time_, issue_event.pid_, issue_event.msg_);
            sprintf(buf,"Total Order: Node %d issue <time:%d, pid:%d, msg:%s>\n", lc_node_num, issue_event.time_, issue_event.pid_, issue_event.msg_);
            save_total_order(lc_node_num, buf);
            printf("\t ---- Node %d issue queue(%d), message queue(%d) issue count(%d) after ACK\n", lc_node_num,lc_order_issue_queue.size_, lc_order_msg_queue.size_,lc_issue_count);
            print_queue(lc_node_num,&lc_order_issue_queue);
          }
        }


      }// end of while(1)
    }
    else { //parent process as the time deamon
      // create recieve service for parent process
      lc_node_num = lc_node_sum;
      pthread_t precv_msg;
      pthread_create(&precv_msg, NULL, lc_recv_service, (void*)(lc_parent_deal_with_msg));

      lc_sync_clock();
    }

    // wait for all child process end
    while ((wpid=waitpid(-1,&status,0))!=-1) {
      printf("Process %d terminated\n",wpid);
      pr_exit(status);
    }

    pthread_exit(NULL);
}
