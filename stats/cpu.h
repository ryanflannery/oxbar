#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/sched.h>

struct cpu_states {
   /*
    * NOTE: See sys/sched.h for CPUSTATES. It's 5, and represenets the # of
    * states tracked in each CPU. They are, in this order, shown with
    * macros defined in sys/sched.h
    *    0  CP_USER  user
    *    1  CP_NICE  nice
    *    2  CP_SYS   system
    *    3  CP_INTR  interrupt
    *    4  CP_IDLL  idle
    */
   float       percentages[CPUSTATES];       /* % time spent in each */
   u_int64_t   raw_ticks[CPUSTATES];         /* raw tick counters for each */
};

struct cpus {
   bool              is_setup;
   int               ncpu;
   struct cpu_states *cpus;
};

extern struct cpus CPUS;

void cpu_init();
void cpu_update();
void cpu_close();

#endif
