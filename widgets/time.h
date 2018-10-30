#ifndef WTIME_H
#define WTIME_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_time_settings {
   char *hdcolor;
   char *bgcolor;
   char *format;
};

struct widget_time_state {
   /* pointers to stuff */
   struct oxstats              *stats;
   struct widget_time_settings *settings;
   /* no local state */
};

void *wtime_init(struct oxstats *, void *settings);
void  wtime_free(void *wstate);
bool  wtime_enabled(void *wstate);
void  wtime_draw(void *wstate, struct xctx *);

#endif
