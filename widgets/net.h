#ifndef WNET_H
#define WNET_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_net_settings {
   char *hdcolor;
   char *chart_bgcolor;
   char *inbound_chart_color_bgcolor;
   char *inbound_chart_color_pgcolor;
   char *outbound_chart_color_bgcolor;
   char *outbound_chart_color_pgcolor;
};

struct widget_net {
   /* pointers to stuff */
   struct oxstats             *stats;
   struct widget_net_settings *settings;
   /* local state */
   struct chart               *inbound;
   struct chart               *outbound;
};

void *wnet_init(struct oxstats *, void *settings);
void  wnet_free(void *widget);
bool  wnet_enabled(void *widget);
void  wnet_draw(void *widget, struct xctx *);

#endif
