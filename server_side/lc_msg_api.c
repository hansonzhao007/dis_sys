#include "mylib.h"
#include "lc_msg_api.h"

lc_msg_queue lc_order_queue;
lc_msg_queue lc_order_issue_queue;

lc_msg_queue lc_FIFO_queue;

lc_msg msg_generate(enum msg_type t, int tm, int pid, char * m) {
  lc_msg msg;
  msg.msg_type_ = t;
  msg.time_ = tm;
  msg.pid_ = pid;
  sprintf(msg.msg_,"%s",m);
  return msg;
}

bool msg_greater(lc_msg* a, lc_msg* b) {
  if(a->time_ > b->time_){ // a 时间戳更大
    return true;
  }
  else if (a->time_ == b->time_ && a->pid_ > b->pid_){ // 时间戳相等, pid 更大
    return true;
  }
  return false;
}

// empty msg type-> CMD,time:0,pid:0
bool is_empty_msg(lc_msg m) {
  if (m.msg_type_ == CMD &&
      m.time_ == 0 &&
      m.pid_ == 0 )
      return true;
  return false;
}

void print_msg(lc_msg m) {
  char buf[64];
  encode_msg(buf, m);
  printf("%s\n",buf);
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

bool is_queue_empty(struct lc_msg_queue* queue){
  return queue->head_ == NULL;
}

void enqueue(lc_msg_queue* queue, lc_msg msg) {
  lc_msg_node* pnode = calloc(1,sizeof(lc_msg_node));
  pnode->msg_ = msg;
  queue->size_++;
  if (queue->order_type_ == PARTIAL) { // partial order, FIFO
    if(queue->head_ == NULL) {
      // insert first node in queue
      queue->znode.next_ = pnode;
      queue->head_ = pnode;
      queue->tail_ = pnode;
      pnode->before_ = &(queue->znode);
      pnode->next_ = NULL;
    }
    else {
      // FIFO, append new node next to tail
      queue->tail_->next_ = pnode;
      pnode->next_ = NULL;
      pnode->before_ = queue->tail_;
      queue->tail_ = pnode;
    }
  }
  else { // totally order
    if (queue->head_ == NULL) {
      // insert first node in queue
      queue->znode.next_ = pnode;
      queue->head_ = pnode;
      queue->tail_ = pnode;
      pnode->before_ = &(queue->znode);
      pnode->next_ = NULL;
    }
    else {
      // insert into queue
      lc_msg_node* pcurrent = queue->head_;
      while(pcurrent != NULL) {
          if (msg_greater(&pnode->msg_, &pcurrent->msg_)) { // 如果新的msg比current大,说明优先级低(timestamp大,或者pid大),往后找,
            pcurrent = pcurrent->next_;
          }
          else { // pnode 比 current 优先级高
            break;
          }
      }
      if (pcurrent == NULL) { // 说明新 msg的 pnode 需要插在queue末尾
          queue->tail_->next_ = pnode;
          pnode->next_ = NULL;
          pnode->before_ = queue->tail_;
          queue->tail_ = pnode;
      } else { // 优先级高则插在current之前
          pnode->before_ = pcurrent->before_;
          pnode->next_ = pcurrent;
          pcurrent->before_->next_ = pnode;
          pcurrent->before_ = pnode;
          if(pcurrent == queue->head_) { // 如果新插入的msg需要插入在 head 之前,那么需要调整 head
            queue->head_ = pnode;
          }
      }
    }
  }
}

lc_msg dequeue(struct lc_msg_queue* queue) {
  lc_msg result;
  lc_msg_node *tmp;
  if(!is_queue_empty(queue)) {
    tmp = queue->head_;
    result = queue->head_->msg_;
    queue->size_--;
    queue->znode.next_ = queue->head_->next_;
    if(queue->head_->next_ != NULL){ // 删除的不是最后一个element,那么只需要重定向 head 就好
      queue->head_->next_->before_ = &queue->znode;
    }
    else{ // 需要 dequeue 的是最后一个 element, tail 和 head 都要变
      queue->tail_ = &queue->znode;
    }
    queue->head_ = queue->znode.next_;
    free(tmp);
  } else {
    result = queue->znode.msg_; // if empty return znode msg
  }
  return result;
}

lc_msg peek(struct lc_msg_queue* queue) {
  if (!is_queue_empty(queue))
    return queue->head_->msg_;
  else
    return queue->znode.msg_; // return znode msg of the queue
}


void queue_init(lc_msg_queue* queue, enum order_type t) {
  queue->size_ = 0;
  queue->order_type_ = t;
  queue->znode.msg_ = msg_generate(CMD,0,0,"0");
  queue->znode.next_ = NULL;
  queue->znode.before_ = NULL;
  queue->head_ = NULL;
  queue->tail_ = &(queue->znode);

  queue->enqueue = enqueue;
  queue->dequeue = dequeue;
  queue->peek = peek;
  queue->isEmpty = is_queue_empty;
}

void print_queue(lc_msg_queue* queue) {
  lc_msg_node* cur = queue->head_;
  while(cur!=NULL) {
    print_msg(cur->msg_);
    cur=cur->next_;
  }
}


//
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
//
//   queue_init(&lc_order_queue, TOTAL);
//   queue_init(&lc_FIFO_queue, PARTIAL);
//
//   for(int i = 0; i < 10; i++) {
//   lc_order_queue.enqueue(&lc_order_queue, m);
//   lc_FIFO_queue.enqueue(&lc_FIFO_queue,m);
//   }
// }
