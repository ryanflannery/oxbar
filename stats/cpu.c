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

#include <sys/types.h>
#include <sys/sysctl.h>

#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"

void
cpu_init(struct cpu_stats *stats)
{
   size_t size;
   int mib[] = { CTL_HW, HW_NCPUONLINE };

   /* get number of cpu's */
   size = sizeof(stats->ncpu);
   if (sysctl(mib, 2, &(stats->ncpu), &size, NULL, 0) == -1)
      err(1, "HW.NCPU");

   /* allocate array of cpu states */
   stats->cpus = calloc(stats->ncpu, sizeof(struct cpu_states));
   if (NULL == stats->cpus)
      err(1, "calloc failed for %d cpus", stats->ncpu);

   stats->is_setup = true;
   cpu_update(stats);   /* to set initial counters */
}

void
cpu_update(struct cpu_stats *stats)
{
   static int  mib[] = { CTL_KERN, KERN_CPTIME2, 0 };
   int         cpu, state;

   for (cpu = 0; cpu < stats->ncpu; cpu++) {

      /* update raw */
      mib[2] = cpu;
      u_int64_t current_ticks[CPUSTATES];
      size_t size = sizeof(current_ticks);
      if (sysctl(mib, 3, current_ticks, &size, NULL, 0) < 0)
         err(1, "KERN.CPTIME2[%d]", cpu);

      /* calculate diffs from last call */
      u_int64_t nticks = 0;
      u_int64_t diffs[CPUSTATES] = { 0 };
      for (state = 0; state < CPUSTATES; state++) {
         if (current_ticks[state] < stats->cpus[cpu].raw_ticks[state]) {
            diffs[state] = INT64_MAX - stats->cpus[cpu].raw_ticks[state]
                         + current_ticks[state];
         } else {
            diffs[state] = current_ticks[state]
                         - stats->cpus[cpu].raw_ticks[state];
         }
         nticks += diffs[state];
      }

      if (nticks == 0)  /* guard divide-by-zero */
         nticks = 1;

      /* calculate percents */
      for (state = 0; state < CPUSTATES; state++) {
         stats->cpus[cpu].percentages[state] = ((diffs[state] * 1000
                                             + (nticks / 2)) / nticks) / 10;
      }

      /* copy current back into CPUS state */
      for (state = 0; state < CPUSTATES; state++)
         stats->cpus[cpu].raw_ticks[state] = current_ticks[state];
   }
}

void
cpu_close(struct cpu_stats *stats)
{
   free(stats->cpus);
}
