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

#include <err.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "net.h"

volatile sig_atomic_t sig_stop = 0;
void stop(int __attribute__((unused)) sig) { sig_stop = 1; }

int
main()
{
	struct net_stats s;
	signal(SIGINT, stop);

	net_init(&s);
	if (!s.is_setup)
		errx(1, "failed to setup core!");

	printf("iface: '%s'\n\n", s.iface);
	printf("%10s %10s %10s %10s %10s %10s %10s %10s\n",
		"#p in", "#p out", "#new in", "#new out",
		"#b in", "#b out", "#b new in", "#b new out");

	while (!sig_stop) {
		net_update(&s);
		printf("%10lu %10lu %10lu %10lu %10llu %10llu %10llu %10llu\n",
			s.packets_in,     s.packets_out,
			s.packets_in_new, s.packets_in_new,
			s.bytes_in,       s.bytes_out,
			s.bytes_in_new,   s.bytes_out_new);
		sleep(1);
	}

	net_close(&s);
}
