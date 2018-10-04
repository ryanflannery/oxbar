#ifndef WIDGETS_H
#define WIDGETS_H

#include "gui.h"
#include "settings.h"
#include "stats/stats.h"
#include "gui/xdraw.h"

/*
 * Global list of all known widget types and how to create them.
 * When adding a widget, they must be added here.
 */
extern widget_t WIDGET_RECIPES[];
extern const size_t NWIDGET_RECIPES;

widget_t* widget_create_from_recipe(
      const char *name,
      settings_t *settings,
      oxstats_t  *stats);

void widgets_init(gui_t *gui, settings_t *settings, oxstats_t *stats);

#endif
