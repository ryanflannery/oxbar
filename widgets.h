#ifndef WIDGETS_H
#define WIDGETS_H

#include "settings.h"
#include "gui/gui.h"
#include "stats/stats.h"

/* this is the state local to each widget */
typedef struct widget_context {
   settings_t *settings;
   oxstats_t  *stats;
#  define MAX_CHARTS_PER_WIDGET 10
   chart_t    *charts[MAX_CHARTS_PER_WIDGET];
} widget_context_t;

/* create & free actual widgets from "names" and then add them to the gui */
void widgets_init(gui_t *gui, settings_t *settings, oxstats_t *stats);
void widgets_free();

#endif
