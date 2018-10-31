#ifndef WNPROCS_H
#define WNPROCS_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_nprocs_settings {
   char *hdcolor;
   char *bgcolor;
   char *fgcolor;
};

bool  wnprocs_enabled(void *wstate);
void  wnprocs_draw(void *wstate, struct xctx *);

#endif
