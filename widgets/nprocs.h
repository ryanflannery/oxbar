#ifndef WNPROCS_H
#define WNPROCS_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_nprocs_settings {
   char *hdcolor;
   char *bgcolor;
};

struct widget_nprocs_state {
   /* pointers to stuff */
   struct oxstats                *stats;
   struct widget_nprocs_settings *settings;
   /* no local state */
};

void *wnprocs_init(struct oxstats *, void *settings);
void  wnprocs_free(void *wstate);
bool  wnprocs_enabled(void *wstate);
void  wnprocs_draw(void *wstate, struct xctx *);

#endif
