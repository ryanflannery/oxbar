#ifndef WCPUSHORT_H
#define WCPUSHORT_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

void *wcpushort_init(struct oxstats *, void *settings);
void  wcpushort_free(void *widget);
bool  wcpushort_enabled(void *widget);
void  wcpushort_draw(void *widget, struct xctx *);

#endif
