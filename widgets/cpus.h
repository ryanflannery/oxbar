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
   char *chart_bgcolor;
   char *chart_color_sys;
   char *chart_color_interrupt;
   char *chart_color_user;
   char *chart_color_nice;
   char *chart_color_spin;
   char *chart_color_idle;
};

struct widget_cpu {
   /* pointers to stuff */
   struct oxstats             *stats;
   struct widget_cpu_settings *settings;
   /* local state */
   size_t                      ncpus;
   struct chart              **cpu_charts;
};

void *wcpu_init(struct oxstats *, void *settings);
void  wcpu_free(void *widget);
bool  wcpu_enabled(void *widget);
void  wcpu_draw(void *widget, struct xctx *);

#endif
