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
#include "cpu.h"

int
main()
{
   int i;
   cpu_init();
   if (!CPUS.is_setup)
      errx(1, "failed to setup cpus!");

   printf("# cpus = %d\n\n", CPUS.ncpu);

   printf("%4s: %7s %7s %7s %7s %7s",
         "cpuX", "user", "nice", "system", "intrpt", "idle\n");

   cpu_update();
   cpu_update();

   while (1)
   {
      cpu_update();
      for (i = 0; i < CPUS.ncpu; i++) {
         printf("cpu%1d: %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%%\n",
               i,
               CPUS.cpus[i].percentages[CP_USER],
               CPUS.cpus[i].percentages[CP_NICE],
               CPUS.cpus[i].percentages[CP_SYS],
               CPUS.cpus[i].percentages[CP_INTR],
               CPUS.cpus[i].percentages[CP_IDLE]);
      }
      printf("\n");

      sleep(1);
   }

   cpu_close();
}
