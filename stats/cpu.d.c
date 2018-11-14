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

#include "cpu.h"

volatile sig_atomic_t sig_stop = 0;
void stop(int __attribute__((unused)) sig) { sig_stop = 1; }

int
main()
{
   struct cpu_stats s;
   int i;
   signal(SIGINT, stop);

   cpu_init(&s);
   if (!s.is_setup)
      errx(1, "failed to setup cpus!");

   printf("#cpus: %d\n\n", s.ncpu);
   printf("%4s: %7s %7s %7s %7s %7s %7s",
         "cpuX", "user", "nice", "system", "spin", "intrpt", "idle\n");

   while (!sig_stop) {
      cpu_update(&s);
      for (i = 0; i < s.ncpu; i++) {
         printf("cpu%1d: %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%%\n",
               i,
               s.cpus[i].percentages[CP_USER],
               s.cpus[i].percentages[CP_NICE],
               s.cpus[i].percentages[CP_SYS],
               s.cpus[i].percentages[CP_SPIN],
               s.cpus[i].percentages[CP_INTR],
               s.cpus[i].percentages[CP_IDLE]);
      }
      sleep(1);
   }

   cpu_close(&s);
}
