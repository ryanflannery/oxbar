#include <err.h>

#include "widgets.h"
#include "gui/xdraw.h"
#include "widgets/battery.h"
#include "widgets/volume.h"
#include "widgets/nprocs.h"
#include "widgets/memory.h"
#include "widgets/cpus.h"
#include "widgets/net.h"
#include "widgets/time.h"

/* this tracks all widgets created */
#define MAX_ALL_WIDGETS 1000
static widget_t *WIDGETS[MAX_ALL_WIDGETS];
static size_t    NWIDGETS = 0;

static widget_t*
find_recipe(const char *name)
{
   size_t i = 0;
   for (; i < NWIDGET_RECIPES; i++) {
      if (0 == strncmp(name, WIDGET_RECIPES[i].name, strlen(WIDGET_RECIPES[i].name)))
         return &WIDGET_RECIPES[i];
   }
   return NULL;
}

widget_t*
widget_create_from_recipe(
      const char *name,
      settings_t *settings,
      oxstats_t  *stats)
{
   widget_t *w, *recipe;

   if (NULL == (recipe = find_recipe(name)))
      errx(1, "no widget recipe named '%s'", name);

   w = malloc(sizeof(widget_t));
   if (NULL == w)
      err(1, "%s: malloc failed", __FUNCTION__);

   w->name     = recipe->name;
   w->enabled  = recipe->enabled;
   w->free     = recipe->free;
   w->draw     = recipe->draw;
   w->hdcolor  = recipe->hdcolor;

   w->context = malloc(sizeof(widget_context_t));
   if (NULL == w->context)
      err(1, "%s: malloc failed", __FUNCTION__);

   w->context->settings = settings;
   w->context->stats    = stats;

   WIDGETS[NWIDGETS++] = w;   /* track to cleanup in widgets_free() */

   return w;
}

void
widgets_free()
{
   size_t i = 0;
   for (; i < NWIDGETS; i++) {
      free(WIDGETS[i]->context);
      WIDGETS[i]->free(WIDGETS[i]);
   }
}

void
widgets_init(gui_t *gui, settings_t *settings, oxstats_t *stats)
{
   gui_add_widget(gui, L2R,
         widget_create_from_recipe("nprocs", settings, stats));
   gui_add_widget(gui, L2R,
         widget_create_from_recipe("cpus", settings, stats));
   gui_add_widget(gui, L2R,
         widget_create_from_recipe("memory", settings, stats));
   gui_add_widget(gui, L2R,
         widget_create_from_recipe("net", settings, stats));

   gui_add_widget(gui, CENTERED,
         widget_create_from_recipe("time", settings, stats));

   gui_add_widget(gui, R2L,
         widget_create_from_recipe("battery", settings, stats));
   gui_add_widget(gui, R2L,
         widget_create_from_recipe("volume", settings, stats));
}

/* handy defaults for simple widgets (those that don't need init/free) */
void widget_free_empty(__attribute__((unused)) widget_t *w) { free(w); }
#define NO_WIDGET_FREE widget_free_empty

/* build the global list of all available widgets */
widget_t WIDGET_RECIPES[] = {
   { "battery", NULL, wbattery_enabled, NO_WIDGET_FREE, wbattery_draw, NULL },
   { "volume",  NULL, wvolume_enabled,  NO_WIDGET_FREE, wvolume_draw,  NULL },
   { "nprocs",  NULL, wnprocs_enabled,  NO_WIDGET_FREE, wnprocs_draw,  NULL },
   { "memory",  NULL, wmemory_enabled,  NO_WIDGET_FREE, wmemory_draw,  NULL },
   { "cpus",    NULL, wcpus_enabled,    NO_WIDGET_FREE, wcpus_draw,    NULL },
   { "net",     NULL, wnet_enabled,     NO_WIDGET_FREE, wnet_draw,     NULL },
   { "time",    NULL, wtime_enabled,    NO_WIDGET_FREE, wtime_draw,    NULL },
};
const size_t NWIDGET_RECIPES = sizeof(WIDGET_RECIPES) / sizeof(widget_t);
