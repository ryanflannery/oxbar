#ifndef NET_H
#define NET_H

#include <stdbool.h>
#include <sys/types.h>

typedef struct net_info {
   bool  is_setup;

   /* TODO Raw # packets in/out isn't used anymore. Remove? */

   /* iface name (first in the 'egress' group) */
   char *iface;

   /* raw counters (these can rollover) */
   u_long   raw_ip_packets_in;
   u_long   raw_ip_packets_out;
   uint64_t raw_bytes_in;
   uint64_t raw_bytes_out;

   /* diff since last update */
   u_long   new_ip_packets_in;
   u_long   new_ip_packets_out;
   uint64_t new_bytes_in;
   uint64_t new_bytes_out;

} net_info_t;

extern net_info_t NET;

void net_init();
void net_update();
void net_close();

#endif
