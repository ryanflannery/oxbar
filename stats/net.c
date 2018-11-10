#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/ip_var.h>

#include "net.h"
#include "util.h"

struct net_info NET;

void
net_init()
{
   NET.iface = get_egress();
   NET.raw_ip_packets_in = NET.raw_ip_packets_out = 0;
   NET.new_ip_packets_in = NET.new_ip_packets_out = 0;
   NET.is_setup = true;
   net_update();
}

void
net_update_packets()
{
   static struct ipstat stats;
	static int mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_STATS };
	static size_t len = sizeof(stats);

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

/*
 * TODO Refactor network bytes logic
 * This could be greatly simplified. I think?
 */
static void
get_rtaddrs(int addrs, struct sockaddr *sa, struct sockaddr **rti_info)
{
	int i;

	for (i = 0; i < RTAX_MAX; i++) {
		if (addrs & (1 << i)) {
			rti_info[i] = sa;
			sa = (struct sockaddr *)((char *)(sa) +
			    roundup(sa->sa_len, sizeof(long)));
		} else
			rti_info[i] = NULL;
	}
}
void
net_update_bytes()
{
   struct rt_msghdr *rtm;
	static int mib[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
   size_t sizeneeded = 0;
   char *buf = NULL;

   if (-1 == sysctl(mib, 6, NULL, &sizeneeded, NULL, 0))
      err(1, "sysctl CTL_NET.PF_ROUTE.0.0.NET_RT_IFLIST.0 failed");

   if (NULL == (buf = malloc(sizeneeded)))
      err(1, "%s: malloc failed", __FUNCTION__);

   if (-1 == sysctl(mib, 6, buf, &sizeneeded, NULL, 0))
      err(1, "%s: sysctl2 failed", __FUNCTION__);

   char *lim = buf + sizeneeded;
   char *next;
   struct if_msghdr  ifm;
   struct if_data   *ifd;
   char name[IFNAMSIZ + 1];
   struct sockaddr *sa, *rti_info[RTAX_MAX];;
   struct sockaddr_dl *sdl;

   for (next = buf; next < lim; next += rtm->rtm_msglen) {
      rtm = (struct rt_msghdr*)next;
      switch (rtm->rtm_type) {
         case RTM_IFINFO:
            bcopy(next, &ifm, sizeof(ifm));
            ifd = &ifm.ifm_data;
            sa = (struct sockaddr*)(next + rtm->rtm_hdrlen);
            get_rtaddrs(ifm.ifm_addrs, sa, rti_info);
            sdl = (struct sockaddr_dl *)rti_info[RTAX_IFP];
            if (NULL == sdl || AF_LINK != sdl->sdl_family) {
               printf("GOT A NULL!");
               continue;
            }
            bzero(name, sizeof(name));
            if (sdl->sdl_nlen >= IFNAMSIZ)
               memcpy(name, sdl->sdl_data, IFNAMSIZ - 1);
            else if (sdl->sdl_nlen > 0)
               memcpy(name, sdl->sdl_data, sdl->sdl_nlen);

            if (strcmp(NET.iface, name))
               break;

            NET.new_bytes_in  = ifd->ifi_ibytes - NET.raw_bytes_in;
            NET.new_bytes_out = ifd->ifi_obytes - NET.raw_bytes_out;
            NET.raw_bytes_in  = ifd->ifi_ibytes;
            NET.raw_bytes_out = ifd->ifi_obytes;
            break;
         case RTM_NEWADDR:
            break;
      }
   }
}

void
net_update()
{
   net_update_packets();
   net_update_bytes();
}

void
net_close()
{
   /* TODO NET cleanup routine should free/cleanup shiz */
}
