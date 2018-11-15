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
#include <sys/types.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net80211/ieee80211.h>
#include <net80211/ieee80211_ioctl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "wifi.h"
#include "util.h"

static int    wifi_socket;
static struct ieee80211_bssid bssid;

void
wifi_init(struct wifi_stats *stats)
{
	int ibssid;
	stats->is_setup = false;

	if (NULL == (stats->iface = get_egress()))
		return;

	if (-1 == (wifi_socket = socket(AF_INET, SOCK_DGRAM, 0))) {
		free(stats->iface);
		return;
	}

	memset(&bssid, 0, sizeof(bssid));
	strlcpy(bssid.i_name, stats->iface, sizeof(bssid.i_name));

	ibssid = ioctl(wifi_socket, SIOCG80211BSSID, &bssid);
	if (0 == ibssid)
		stats->is_setup = true;   /* egress is a 802.11, we're good */
	else {
		stats->is_setup = false;  /* that's not the case - bail */
		free(stats->iface);
	}
}

void
wifi_update(struct wifi_stats *stats)
{
	if (!stats->is_setup)
		return;

	struct ieee80211_nodereq req;
	bzero(&req, sizeof(req));

	bcopy(bssid.i_bssid, &req.nr_macaddr, sizeof(req.nr_macaddr));
	strlcpy(req.nr_ifname, stats->iface, sizeof(req.nr_ifname));
	if (0 == ioctl(wifi_socket, SIOCG80211NODE, &req) && req.nr_rssi)
		stats->signal_strength = (float) IEEE80211_NODEREQ_RSSI(&req);
}

void
wifi_close(struct wifi_stats *stats)
{
	if (stats->is_setup) {
		free(stats->iface);
		close(wifi_socket);
	}
}
