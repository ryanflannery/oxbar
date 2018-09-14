#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>

#include "net.h"

net_info_t NET;

void
net_init()
{
   NET.raw_ip_packets_in = NET.raw_ip_packets_out = 0;
   NET.new_ip_packets_in = NET.new_ip_packets_out = 0;
   NET.is_setup = true;
   net_update();
}

void
net_update()
{
   struct ipstat stats;
	int mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_STATS };
	size_t len = sizeof(stats);

	if (-1 == sysctl(mib, sizeof(mib) / sizeof(mib[0]), &stats, &len, NULL, 0))
      err(1, "sysctl CTL_NET.PF_INET.IPPROTO_IP.IPCTL_STATS failed");

   if (stats.ips_total < NET.raw_ip_packets_in)
      NET.new_ip_packets_in  = ULONG_MAX - NET.raw_ip_packets_in + stats.ips_total;
   else
      NET.new_ip_packets_in  = stats.ips_total - NET.raw_ip_packets_in;

   if (stats.ips_localout < NET.raw_ip_packets_out)
      NET.new_ip_packets_out = ULONG_MAX - NET.raw_ip_packets_out + stats.ips_localout;
   else
      NET.new_ip_packets_out = stats.ips_localout - NET.raw_ip_packets_out;

   NET.raw_ip_packets_in  = stats.ips_total;
   NET.raw_ip_packets_out = stats.ips_localout;
}

void
net_close()
{
   /* TODO free shiz? */
}
