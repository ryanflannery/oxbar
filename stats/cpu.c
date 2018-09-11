#include <err.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/sched.h>
#include <sys/swap.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/vmmeter.h>

#include "cpu.h"

cpus_t CPUS;

void
cpu_init()
{
   size_t size;
   int mib[] = { CTL_HW, HW_NCPU };

   /* get number of cpu's */
   size = sizeof(CPUS.ncpu);
   if (sysctl(mib, 2, &(CPUS.ncpu), &size, NULL, 0) == -1)
      err(1, "sysinfo init: sysctl HW.NCPU failed");

   /* allocate array of cpu states */
   CPUS.cpus = calloc(CPUS.ncpu, sizeof(cpu_states_t));
   if (NULL == CPUS.cpus)
      err(1, "core_init: calloc failed for %d cpus", CPUS.ncpu);

   CPUS.is_setup = true;
}

void
cpu_update()
{
   static int  mib[] = { CTL_KERN, 0, 0 };
   int         cpu, state;

   /* get RAW cpu ticks & update percentages */
   if (CPUS.ncpu > 1) {
      mib[1] = KERN_CPTIME2;
      for (cpu = 0; cpu < CPUS.ncpu; cpu++) {
         /* update raw */
         mib[2] = cpu;
         u_int64_t current_ticks[CPUSTATES];
         size_t size = sizeof(current_ticks);
         if (sysctl(mib, 3, current_ticks, &size, NULL, 0) < 0)
            err(1, "cpu_update: KERN.CPTIME2.%d failed", cpu);

         /* calculate diffs from last call */
         u_int64_t nticks = 0;
         u_int64_t diffs[CPUSTATES] = { 0 };
         for (state = 0; state < CPUSTATES; state++) {
            if (current_ticks[state] < CPUS.cpus[cpu].raw_ticks[state]) {
               diffs[state] = INT64_MAX
                            - CPUS.cpus[cpu].raw_ticks[state]
                            + current_ticks[state];
            } else {
               diffs[state] = current_ticks[state]
                            - CPUS.cpus[cpu].raw_ticks[state];
            }
            nticks += diffs[state];
         }

         if (nticks == 0) nticks = 1;

         /* calculate percents */
         for (state = 0; state < CPUSTATES; state++) {
            CPUS.cpus[cpu].percentages[state] = ((diffs[state] * 1000 + (nticks / 2)) / nticks) / 10;
         }

         /* copy current back into CPUS state */
         for (state = 0; state < CPUSTATES; state++)
            CPUS.cpus[cpu].raw_ticks[state] = current_ticks[state];
      }
   } else {
      /* update raw */
      mib[1] = KERN_CPTIME;
      long current_ticks[CPUSTATES];
      size_t size = sizeof(current_ticks);
      if (sysctl(mib, 2, current_ticks, &size, NULL, 0) < 0)
         err(1, "core_update: KERN.CPTIME failed");

      /* calculate diffs from last call */
      long nticks = 0;
      long diffs[CPUSTATES] = { 0 };
      for (state = 0; state < CPUSTATES; state++) {
         if (current_ticks[state] < (long)CPUS.cpus[0].raw_ticks[state]) {
            diffs[state] = INT64_MAX
                         - CPUS.cpus[0].raw_ticks[state]
                         + current_ticks[state];
         } else {
            diffs[state] = current_ticks[state]
                         - CPUS.cpus[0].raw_ticks[state];
         }
         nticks += diffs[state];
      }

      if (nticks == 0) nticks = 1;

      /* update percents */
         for (state = 0; state < CPUSTATES; state++) {
            CPUS.cpus[0].percentages[state] = ((diffs[state] * 1000 + (nticks / 2)) / nticks) / 10;
         }

      /* copy current back into CPUS state */
      for (state = 0; state < CPUSTATES; state++)
         CPUS.cpus[0].raw_ticks[state] = current_ticks[state];
   }

   /* update string versions */
   for (cpu = 0; cpu < CPUS.ncpu; cpu++) {
      for (state = 0; state < CPUSTATES; state++) {
         if (NULL != CPUS.cpus[cpu].str_percentages[state])
            free(CPUS.cpus[cpu].str_percentages[state]);
      }
   }
}

void
cpu_close()
{
   /* TODO: free shiz */
}
