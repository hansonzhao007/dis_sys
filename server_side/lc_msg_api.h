#ifndef LC_MSG_API
#define LC_MSG_API
#include "mylib.h"
enum msg_type {
  CMD,
  MSG
};

enum order_type {
  PARTIAL,
  TOTAL
};

typedef struct lc_msg {
  char msg_[32];
  int time_;
  int pid_;
  enum msg_type msg_type_;
  int ack_time_;
}lc_msg;

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
  int size_;
  enum order_type order_type_; // decide it is a priority queue or FIFO
  lc_msg_node znode;
  lc_msg_node* head_;
  lc_msg_node* tail_;
  void (*enqueue)(struct lc_msg_queue* queue, lc_msg);
  lc_msg (*dequeue)(struct lc_msg_queue* queue);
  lc_msg (*peek)(struct lc_msg_queue* queue);
  bool (*isEmpty)(struct lc_msg_queue* queue);
}lc_msg_queue;

struct lc_msg msg_generate(enum msg_type t, int tm, int pid, char * m);
void encode_msg(char* buf,struct lc_msg msg);
void decode_msg(char* str,struct lc_msg* pmsg);
void queue_init(lc_msg_queue* queue, enum order_type t);
void print_msg(lc_msg m);

extern lc_msg_queue lc_order_queue;
extern lc_msg_queue lc_order_issue_queue;
extern lc_msg_queue lc_FIFO_queue;
#endif
