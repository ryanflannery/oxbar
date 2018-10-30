#ifndef WVOLUME_H
#define WVOLUME_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_volume_settings {
   char *hdcolor;
   int   chart_width;
   char *chart_bgcolor;
   char *chart_pgcolor;
};

struct widget_volume {
   /* pointers to stuff */
   struct oxstats                 *stats;
   struct widget_volume_settings *settings;
   /* no local state */
};

void *wvolume_init(struct oxstats *, void *settings);
void  wvolume_free(void *widget);
bool  wvolume_enabled(void *widget);
void  wvolume_draw(void *widget, struct xctx *);

#endif
