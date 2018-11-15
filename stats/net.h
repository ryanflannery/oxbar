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

#ifndef NET_H
#define NET_H

#include <sys/types.h>

#include <stdbool.h>

struct net_stats {
	bool  is_setup;

	/* iface name (first in the 'egress' group) */
	char *iface;

	/* raw packet & byte counters (these can rollover) */
	u_long   packets_in, packets_out;
	uint64_t bytes_in,   bytes_out;

	/* diff since last update (handles rollover) */
	u_long   packets_in_new, packets_out_new;
	uint64_t bytes_in_new,   bytes_out_new;
};

void net_init(struct net_stats*);
void net_update(struct net_stats*);
void net_close(struct net_stats*);

#endif
