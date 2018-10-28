#ifndef WIDGETS_H
#define WIDGETS_H

#include "settings.h"
#include "gui/gui.h"
#include "stats/stats.h"

/* this is the state local to each widget */
struct widget_context {
   struct settings *settings;
   struct oxstats  *stats;
#  define MAX_CHARTS_PER_WIDGET 10
   struct chart    *charts[MAX_CHARTS_PER_WIDGET];
};

/* create & free actual widgets from "names" and then add them to the gui */
void widgets_init(struct gui *gui, struct settings *settings, struct oxstats *stats);
void widgets_free();

#endif
