#ifndef WCPUS_H
#define WCPUS_H

#include <stdbool.h>
#include "../gui/xdraw.h"
#include "../stats/stats.h"

/*
 * settings for a cpus widget (public so settings.* component can pick them
 * up and set them via cli or config file)
 */
struct widget_cpu_settings {
	char *hdcolor;
	char *bgcolor;
	char *fgcolor;
	char *chart_bgcolor;
	char *chart_color_system;
	char *chart_color_interrupt;
	char *chart_color_user;
	char *chart_color_nice;
	char *chart_color_spin;
	char *chart_color_idle;
};

struct widget_cpu_state {
	/* pointers to stuff */
	struct oxstats             *stats;
	struct widget_cpu_settings *settings;
	/* local state */
	size_t                      ncpus;
	struct chart              **cpu_charts;
};

void *wcpu_init(struct oxstats*, void *settings);
void  wcpu_free(void *wstate);
bool  wcpu_enabled(void *wstate);
void  wcpu_draw(void *wstate, struct xctx*);

#endif
