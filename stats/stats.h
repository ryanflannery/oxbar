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

#ifndef STATS_H
#define STATS_H

#include "battery.h"
#include "brightness.h"
#include "cpu.h"
#include "memory.h"
#include "net.h"
#include "nprocs.h"
#include "volume.h"
#include "wifi.h"

struct oxstats {
	struct battery_stats    battery;
	struct brightness_stats brightness;
	struct cpu_stats        cpus;
	struct memory_stats     memory;
	struct net_stats        network;
	struct nprocs_stats     nprocs;
	struct volume_stats     volume;
	struct wifi_stats       wifi;
};

extern struct oxstats OXSTATS;

void stats_init();
void stats_update();
void stats_close();

#endif
