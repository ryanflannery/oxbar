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

#include "stats.h"

struct oxstats OXSTATS;

void
stats_init()
{
   battery_init(&OXSTATS.battery);
   brightness_init(&OXSTATS.brightness);
   cpu_init(&OXSTATS.cpus);
   memory_init(&OXSTATS.memory);
   net_init(&OXSTATS.network);
   nprocs_init(&OXSTATS.nprocs);
   volume_init(&OXSTATS.volume);
   wifi_init(&OXSTATS.wifi);

   /* do first update to initialize everything */
   stats_update();
}

void
stats_update()
{
   battery_update(&OXSTATS.battery);
   brightness_update(&OXSTATS.brightness);
   cpu_update(&OXSTATS.cpus);
   memory_update(&OXSTATS.memory);
   net_update(&OXSTATS.network);
   nprocs_update(&OXSTATS.nprocs);
   volume_update(&OXSTATS.volume);
   wifi_update(&OXSTATS.wifi);
}

void
stats_close()
{
   battery_close(&OXSTATS.battery);
   brightness_close(&OXSTATS.brightness);
   cpu_close(&OXSTATS.cpus);
   memory_close(&OXSTATS.memory);
   net_close(&OXSTATS.network);
   nprocs_close(&OXSTATS.nprocs);
   volume_close(&OXSTATS.volume);
   wifi_close(&OXSTATS.wifi);
}
