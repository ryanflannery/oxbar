#ifndef WCPUSLONG_H
#define WCPUSLONG_H

#include <stdbool.h>
#include "cpus.h"
#include "../gui/xdraw.h"
#include "../stats/stats.h"

void *wcpulong_init(struct oxstats *, void *settings);
void  wcpulong_free(void *wstate);
bool  wcpulong_enabled(void *wstate);
void  wcpulong_draw(void *wstate, struct xctx *);

#endif
