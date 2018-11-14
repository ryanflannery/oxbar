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
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "memory.h"

volatile sig_atomic_t sig_stop = 0;
void stop(int __attribute__((unused)) sig) { sig_stop = 1; }

int
main()
{
   struct memory_stats s;
   signal(SIGINT, stop);

   memory_init(&s);
   if (!s.is_setup)
      errx(1, "failed to setup memory!");

   printf("%8s %7s ",   "Act kb",   "%");
   printf("%8s %7s ",   "Tot kb",   "%");
   printf("%8s %7s ",   "Free kb",  "%");
   printf("%8s %7s ",   "Swap U",   "%");
   printf("%8s\n",      "Swap T");

   while (!sig_stop) {
      memory_update(&s);
      printf("%8d %7.1f ", s.active,      s.active_pct);
      printf("%8d %7.1f ", s.total,       s.total_pct);
      printf("%8d %7.1f ", s.free,        s.free_pct);
      printf("%8d %7.1f ", s.swap_used,   s.swap_used_pct);
      printf("%8d\n",      s.swap_total);
      sleep(1);
   }

   memory_close(&s);
}
