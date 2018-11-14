#ifndef WTIME_H
#define WTIME_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_time_settings {
	char *hdcolor;
	char *bgcolor;
	char *fgcolor;
	char *format;
};

bool  wtime_enabled(void *wstate);
void  wtime_draw(void *wstate, struct xctx*);

#endif
