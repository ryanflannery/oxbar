#ifndef WIDGETS_H
#define WIDGETS_H

#include "settings.h"
#include "gui/gui.h"
#include "stats/stats.h"

/* create & free actual widgets from the settings->widgets string */
void widgets_init(struct gui *, struct settings *, struct oxstats *);
void widgets_free();

#endif
