#ifndef WIDGETS_H
#define WIDGETS_H

#include "gui.h"
#include "settings.h"
#include "stats/stats.h"

/* this is state local to each widget */
typedef struct widget_context {
   settings_t *settings;
   oxstats_t  *stats;
#  define MAX_CHARTS_PER_WIDGET 10
   chart_t    *charts[MAX_CHARTS_PER_WIDGET];
} widget_context_t;

/*
 * Global list of all known widget types and how to create them.
 * When adding a widget, they must be added here.
 */
typedef struct widget_recipe {
   widget_t widget;
   void (*init)(struct widget*);
   void (*free)(struct widget*);
} widget_recipe_t;

extern widget_recipe_t WIDGET_RECIPES[];
extern const size_t NWIDGET_RECIPES;

/* creates a new widget_t based on a named recipe (if found) */
widget_t* widget_create_from_recipe(
      const char *name,
      settings_t *settings,
      oxstats_t  *stats);

/* init all widgets / free all created widgets (tracked internally) */
void widgets_init(gui_t *gui, settings_t *settings, oxstats_t *stats);
void widgets_free();

#endif
