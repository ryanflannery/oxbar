#ifndef WBATTERY_H
#define WBATTERY_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_battery_settings {
   char *hdcolor;
   char *bgcolor;
   char *fgcolor;
   char *fgcolor_unplugged;
   int   chart_width;
   char *chart_bgcolor;
   char *chart_pgcolor;
};

struct widget_battery_state {
   /* pointers to stuff */
   struct oxstats                 *stats;
   struct widget_battery_settings *settings;
   /* no local state */
};

void *wbattery_init(struct oxstats *, void *settings);
void  wbattery_free(void *wstate);
bool  wbattery_enabled(void *wstate);
void  wbattery_draw(void *wstate, struct xctx *);

#endif
