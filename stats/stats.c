#include "stats.h"

oxstats_t OXSTATS;

void
stats_init()
{
   battery_init();
   cpu_init();
   memory_init();
   net_init();
   nprocs_init();
   volume_init();

   OXSTATS.battery   = &BATTERY;
   OXSTATS.cpus      = &CPUS;
   OXSTATS.memory    = &MEMORY;
   OXSTATS.network   = &NET;
   OXSTATS.nprocs    = &NPROCS;
   OXSTATS.volume    = &VOLUME;
}

void
stats_update()
{
   battery_update();
   cpu_update();
   memory_update();
   net_update();
   nprocs_update();
   volume_update();
}

void
stats_close()
{
   battery_close();
   cpu_close();
   memory_close();
   net_close();
   nprocs_close();
   volume_close();
}
