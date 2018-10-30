#ifndef WCPUSLONG_H
#define WCPUSLONG_H

#include <stdbool.h>
#include "cpus.h"
#include "../gui/xdraw.h"
#include "../stats/stats.h"

void *wcpulong_init(struct oxstats *, void *settings);
void  wcpulong_free(void *widget);
bool  wcpulong_enabled(void *widget);
void  wcpulong_draw(void *widget, struct xctx *);

#endif
