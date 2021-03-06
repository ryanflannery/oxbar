/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

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

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <strings.h>

#include "net.h"
#include "util.h"

/* private functions */
static void net_update_packets(struct net_stats*);
static void get_rtaddrs(int, struct sockaddr*, struct sockaddr**);
static void net_update_bytes(struct net_stats*);

void
net_init(struct net_stats *stats)
{
	stats->iface = get_egress();
	stats->is_setup = NULL != stats->iface;
	if (stats->is_setup)
		net_update(stats);   /* to set initial counters */
}

static void
net_update_packets(struct net_stats *stats)
{
	static struct ipstat current;
	static int mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_STATS };
	static size_t len = sizeof(current);

	if (-1 == sysctl(mib, sizeof(mib) / sizeof(mib[0]), &current, &len, NULL, 0))
		err(1, "CTL_NET.PF_INET.IPPROTO_IP.IPCTL_STATS");

	/* update inbound packets */
	if (current.ips_total < stats->packets_in)
		stats->packets_in_new = ULONG_MAX - stats->packets_in
		                      + current.ips_total;
	else
		stats->packets_in_new = current.ips_total - stats->packets_in;

	/* update outbound packets */
	if (current.ips_localout < stats->packets_out)
		stats->packets_out_new = ULONG_MAX - stats->packets_out
		                       + current.ips_localout;
	else
		stats->packets_out_new = current.ips_localout
		                       - stats->packets_out;

	stats->packets_in = current.ips_total;
	stats->packets_out = current.ips_localout;
}

/*
 * TODO Refactor network bytes logic below
 * I'm pretty sure this could be greatly simplified
 */
static void
get_rtaddrs(int addrs, struct sockaddr *sa, struct sockaddr **rti_info)
{
	int i;

	for (i = 0; i < RTAX_MAX; i++) {
		if (addrs & (1 << i)) {
			rti_info[i] = sa;
			sa = (struct sockaddr *)((char *)(sa)
			   + roundup(sa->sa_len, sizeof(long)));
		} else
			rti_info[i] = NULL;
	}
}

static void
net_update_bytes(struct net_stats *stats)
{
	struct rt_msghdr *rtm;
	static int        mib[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
	size_t            sizeneeded = 0;
	char             *buf = NULL;

	if (-1 == sysctl(mib, 6, NULL, &sizeneeded, NULL, 0))
		err(1, "sysctl CTL_NET.PF_ROUTE.0.0.NET_RT_IFLIST.0 failed");

	if (NULL == (buf = malloc(sizeneeded)))
		err(1, "%s: malloc failed", __FUNCTION__);

	if (-1 == sysctl(mib, 6, buf, &sizeneeded, NULL, 0))
		err(1, "%s: sysctl2 failed", __FUNCTION__);

	char             *lim = buf + sizeneeded;
	char             *next;
	struct if_msghdr  ifm;
	struct if_data   *ifd;
	char              name[IFNAMSIZ + 1];
	struct sockaddr  *sa, *rti_info[RTAX_MAX];;
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
			if (NULL == sdl || AF_LINK != sdl->sdl_family)
				continue;

			bzero(name, sizeof(name));
			if (sdl->sdl_nlen >= IFNAMSIZ)
				memcpy(name, sdl->sdl_data, IFNAMSIZ - 1);
			else if (sdl->sdl_nlen > 0)
				memcpy(name, sdl->sdl_data, sdl->sdl_nlen);

			if (strcmp(stats->iface, name))
				break;

			stats->bytes_in_new  = ifd->ifi_ibytes
			                     - stats->bytes_in;
			stats->bytes_out_new = ifd->ifi_obytes
			                     - stats->bytes_out;
			stats->bytes_in      = ifd->ifi_ibytes;
			stats->bytes_out     = ifd->ifi_obytes;
			break;

		case RTM_NEWADDR:
			break;
		}
	}
	free(buf);
}

void
net_update(struct net_stats *stats)
{
	if (!stats->is_setup)
		return;

	net_update_packets(stats);
	net_update_bytes(stats);
}

void
net_close(struct net_stats *stats)
{
	if (stats->is_setup)
		free(stats->iface);
}
