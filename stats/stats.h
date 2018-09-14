#ifndef STATS_H
#define STATS_H

#include "battery.h"
#include "cpu.h"
#include "memory.h"
#include "net.h"
#include "nprocs.h"
#include "volume.h"

void stats_init();
void stats_update();
void stats_close();

#endif
