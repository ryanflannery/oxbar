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
#include <stdio.h>
#include <unistd.h>
#include "battery.h"

int
main()
{
   struct battery_stats s;
   battery_init(&s);
   if (!s.is_setup)
      errx(1, "failed to setup battery!");

   printf("%8s\t%8s\t%8s\n", "plugged?", "%", "minutes");

   while (1)
   {
      battery_update(&s);
      printf("%8s\t%8.1f\t%8d\n",
            s.plugged_in ? "TRUE" : "false",
            s.charge_pct,
            s.minutes_remaining);

      sleep(1);
   }

   battery_close(&s);
}
