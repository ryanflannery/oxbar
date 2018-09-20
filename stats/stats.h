#ifndef STATS_H
#define STATS_H

#include "battery.h"
#include "cpu.h"
#include "memory.h"
#include "net.h"
#include "nprocs.h"
#include "volume.h"

typedef struct oxstats {
   battery_info_t   *battery;
   cpus_t           *cpus;
   memory_info_t    *memory;
   net_info_t       *network;
   nprocs_info_t    *nprocs;
   volume_info_t    *volume;
} oxstats_t;

extern oxstats_t OXSTATS;

void stats_init();
void stats_update();
void stats_close();

#endif
