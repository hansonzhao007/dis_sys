#ifndef LG_UTIL_H
#define LG_UTIL_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "lc_msg_api.h"

void lc_node_deal_with_msg(struct lc_msg msg);
void lc_parent_deal_with_msg(struct lc_msg msg);

void lc_broadcast_msg(struct lc_msg msg);
void *lc_recv_service(void* data);
void pr_exit(int status);
void lc_sync_clock(void);
void lc_init_clock(void);

int lc_node_sum;
int lc_node_num;
int lc_logic_clock;
bool is_clock_sync;

#endif
