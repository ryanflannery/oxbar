#ifndef WMEMORY_H
#define WMEMORY_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

struct widget_memory_settings {
	char *hdcolor;
	char *bgcolor;
	char *fgcolor;
	char *chart_bgcolor;
	char *chart_color_free;
	char *chart_color_total;
	char *chart_color_active;
};

struct widget_memory_state {
	/* pointers to stuff */
	struct oxstats                *stats;
	struct widget_memory_settings *settings;
	/* local state */
	struct chart                  *chart;
};

void *wmemory_init(struct oxstats*, void *settings);
void  wmemory_free(void *wstate);
bool  wmemory_enabled(void *wstate);
void  wmemory_draw(void *wstate, struct xctx*);

#endif
