#ifndef LC_MSG_API
#define LC_MSG_API

enum msg_type {
  CMD,
  MSG
};
struct lc_msg {
  char msg_[32];
  int time_;
  int pid_;
  enum msg_type msg_type_;
  int ack_time_;
};
struct lc_msg msg_generate(enum msg_type t, int tm, int pid, char * m);
void encode_msg(char* buf,struct lc_msg msg);
void decode_msg(char* str,struct lc_msg* pmsg);

#endif
