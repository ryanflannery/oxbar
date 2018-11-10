#ifndef WWIFI_H
#define WWIFI_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_wifi_settings {
   char *hdcolor;
   char *bgcolor;
   char *fgcolor;
};

bool  wwifi_enabled(void *wstate);
void  wwifi_draw(void *wstate, struct xctx *);

#endif
