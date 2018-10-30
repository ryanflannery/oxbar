#ifndef WTIME_H
#define WTIME_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_time_settings {
   char *hdcolor;
   char *format;
};

struct widget_time {
   /* pointers to stuff */
   struct oxstats              *stats;
   struct widget_time_settings *settings;
   /* no local state */
};

void *wtime_init(struct oxstats *, void *settings);
void  wtime_free(void *widget);
bool  wtime_enabled(void *widget);
void  wtime_draw(void *widget, struct xctx *);

#endif
