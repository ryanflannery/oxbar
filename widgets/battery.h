#ifndef WBATTERY_H
#define WBATTERY_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_battery_settings {
   char *hdcolor;
   char *fgcolor_unplugged;
   int   chart_width;
   char *chart_bgcolor;
   char *chart_pgcolor;
};

struct widget_battery {
   /* pointers to stuff */
   struct oxstats             *stats;
   struct widget_battery_settings *settings;
   /* no local state */
};

void *wbattery_init(struct oxstats *, void *settings);
void  wbattery_free(void *widget);
bool  wbattery_enabled(void *widget);
void  wbattery_draw(void *widget, struct xctx *);

#endif
