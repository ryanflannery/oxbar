#ifndef WVOLUME_H
#define WVOLUME_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_volume_settings {
   char *hdcolor;
   char *bgcolor;
   char *fgcolor;
   int   chart_width;
   char *chart_bgcolor;
   char *chart_pgcolor;
};

bool  wvolume_enabled(void *wstate);
void  wvolume_draw(void *wstate, struct xctx *);

#endif
