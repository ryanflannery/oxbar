#ifndef WIDGETS_H
#define WIDGETS_H

#include "settings.h"
#include "gui/gui.h"
#include "stats/stats.h"

/* create & free actual widgets from the settings->widgets string */
void widgets_init(struct gui *, struct settings *, struct oxstats *);
void widgets_free();

void widget_set_hdcolor(const char * const name, char **color);
void widget_set_bgcolor(const char * const name, char **color);
void widget_set_fgcolor(const char * const name, char **color);

#endif
