#ifndef WNPROCS_H
#define WNPROCS_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_nprocs_settings {
   char *hdcolor;
};

struct widget_nprocs {
   /* pointers to stuff */
   struct oxstats                *stats;
   struct widget_nprocs_settings *settings;
   /* no local state */
};

void *wnprocs_init(struct oxstats *, void *settings);
void  wnprocs_free(void *widget);
bool  wnprocs_enabled(void *widget);
void  wnprocs_draw(void *widget, struct xctx *);

#endif
