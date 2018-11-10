#ifndef STATS_H
#define STATS_H

#include "battery.h"
#include "cpu.h"
#include "memory.h"
#include "net.h"
#include "nprocs.h"
#include "volume.h"
#include "wifi.h"

struct oxstats {
   struct battery_info  *battery;
   struct cpus          *cpus;
   struct memory_info   *memory;
   struct net_info      *network;
   struct nprocs_info   *nprocs;
   struct volume_info   *volume;
   struct wifi_info     *wifi;
};

extern struct oxstats OXSTATS;

void stats_init();
void stats_update();
void stats_close();

#endif
