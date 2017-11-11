#include "mylib.h"
#include "lc_msg_api.h"
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
