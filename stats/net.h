#ifndef NET_H
#define NET_H

#include <stdbool.h>

typedef struct net_info {
   bool  is_setup;

   /* raw counters (these can rollover) */
   u_long  raw_ip_packets_in;
   u_long  raw_ip_packets_out;

   /* diff since last update */
   u_long  new_ip_packets_in;
   u_long  new_ip_packets_out;

} net_info_t;

extern net_info_t NET;

void net_init();
void net_update();
void net_close();

#endif
