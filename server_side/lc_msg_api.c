#include "mylib.h"
#include "lc_msg_api.h"


lc_msg msg_generate(enum msg_type t, int tm, int pid, char * m) {
  lc_msg msg;
  msg.msg_type_ = t;
  msg.time_ = tm;
  msg.pid_ = pid;
  sprintf(msg.msg_,"%s",m);
  return msg;
}
typedef struct lc_msg_node {
  lc_msg msg_;
  struct lc_msg_node* next_;
  struct lc_msg_node* before_;
} lc_msg_node;

typedef struct lc_msg_queue{
  //    tail_    head_
  //   znode --> NULL
  //
  //             head_
  //             tail_
  //   znode --> node --> NULL
  enum order_type order_type_; // decide it is a priority queue or FIFO
  lc_msg_node znode;
  lc_msg_node* head_;
  lc_msg_node* tail_;
  void (*enqueue)(struct lc_msg_queue* queue, lc_msg);
  lc_msg (*dequeue)(struct lc_msg_queue* queue);
  lc_msg (*peek)(struct lc_msg_queue* queue);
  bool (*isEmpty)(struct lc_msg_queue* queue);
}lc_msg_queue;

void enqueue(lc_msg_queue* queue, lc_msg msg) {
  lc_msg_node* pnode = calloc(1,sizeof(lc_msg_node));
  pnode->msg_ = msg;
  if (queue->order_type_ == PARTIAL) { // partial order, FIFO
    if(queue->head_ == NULL) {
      // insert first node in queue
      queue->znode.next_ = pnode;
      queue->znode.before_ = NULL;
      queue->head_ = pnode;
      queue->tail_ = pnode;
      pnode->next_ = NULL;
    }
    else {
      // FIFO, append new node next to tail
      pnode->next_ = NULL;
      pnode->before_ = queue->tail_;
      queue->tail_ = pnode;
    }
  }
  else { // totally order
    if (queue->head_ == NULL) {
      // insert first node in queue
      queue->znode.next_ = pnode;
      queue->znode.before_ = NULL;
      queue->head_ = pnode;
      queue->tail_ = pnode;
      pnode->next_ = NULL;
    }
    else {
      // insert into queue
      lc_msg_node* pcurrent = queue->head_;
      while(pcurrent != NULL) {


      }
    }
  }
}

// msg is encode like: msg_type-time-pid-msg
void encode_msg(char* buf,struct lc_msg msg) {
  sprintf(buf,"%d-%d-%d-%s",msg.msg_type_, msg.time_, msg.pid_, msg.msg_);
}

void decode_msg(char* str,struct lc_msg* pmsg) {
  char* type = strtok(str, "-");
  char* t = strtok(NULL,"-");
  char* pid = strtok(NULL,"-");
  char* msg = strtok(NULL,"-");
  pmsg->msg_type_ = atoi(type);
  pmsg->time_ = atoi(t);
  pmsg->pid_  = atoi(pid);
  sprintf(pmsg->msg_,"%s",msg);
}

// int main(void) {
//   char buffer[256];
//   struct lc_msg m,m1;
//   m.msg_type_ = CMD;
//   m.pid_ = 123;
//   m.time_ = 222;
//   sprintf(m.msg_, "%s", "Test_msg");
//
//   encode_msg(buffer, m);
//   printf("%s\n",buffer );
//
//   decode_msg(buffer,&m1);
//   printf("decode:\n type:%d\n time:%d\n pid:%d\n msg:%s\n",m1.msg_type_, m1.time_, m1.pid_, m1.msg_);
// }
