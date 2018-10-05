#include <err.h>

#include "gui/xdraw.h"
#include "widgets.h"
#include "widgets/battery.h"
#include "widgets/volume.h"
#include "widgets/nprocs.h"
#include "widgets/memory.h"
#include "widgets/cpus.h"
#include "widgets/net.h"
#include "widgets/time.h"

/* build the global list of all available widgets */
widget_recipe_t WIDGET_RECIPES[] = {
   {{"battery", NULL, wbattery_enabled, wbattery_draw, NULL}, NULL, NULL},
   {{"volume",  NULL, wvolume_enabled,  wvolume_draw,  NULL}, NULL, NULL},
   {{"nprocs",  NULL, wnprocs_enabled,  wnprocs_draw,  NULL}, NULL, NULL},
   {{"memory",  NULL, wmemory_enabled,  wmemory_draw,  NULL}, wmemory_init, wmemory_free },
   {{"cpus",    NULL, wcpus_enabled,    wcpus_draw,    NULL}, wcpus_init,   wcpus_free },
   {{"net",     NULL, wnet_enabled,     wnet_draw,     NULL}, wnet_init,    wnet_free },
   {{"time",    NULL, wtime_enabled,    wtime_draw,    NULL}, NULL, NULL},
};
const size_t NWIDGET_RECIPES = sizeof(WIDGET_RECIPES) / sizeof(widget_recipe_t);

/* this tracks all widgets created */
#define MAX_ALL_WIDGETS 1000
static widget_t *WIDGETS[MAX_ALL_WIDGETS];
static size_t    NWIDGETS = 0;

static widget_recipe_t*
find_recipe(const char *name)
{
   size_t i = 0;
   for (; i < NWIDGET_RECIPES; i++) {
      if (0 == strncmp(name, WIDGET_RECIPES[i].widget.name,
               strlen(WIDGET_RECIPES[i].widget.name)))
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
   widget_recipe_t *recipe;
   widget_t *w;

   if (NULL == (recipe = find_recipe(name)))
      errx(1, "no widget recipe named '%s'", name);

   w = malloc(sizeof(widget_t));
   if (NULL == w)
      err(1, "%s: malloc failed", __FUNCTION__);

   w->name     = recipe->widget.name;
   w->hdcolor  = recipe->widget.hdcolor;
   w->enabled  = recipe->widget.enabled;
   w->draw     = recipe->widget.draw;

   w->context = malloc(sizeof(widget_context_t));
   if (NULL == w->context)
      err(1, "%s: malloc failed", __FUNCTION__);

   w->context->settings = settings;
   w->context->stats    = stats;
   bzero(w->context->charts, sizeof(w->context->charts));

   if (NULL != recipe->init)
      recipe->init(w);

   WIDGETS[NWIDGETS++] = w;   /* track to cleanup in widgets_free() */

   return w;
}

void
widgets_free()
{
   size_t i = 0;

   for (; i < NWIDGETS; i++) {
      widget_recipe_t *recipe = find_recipe(WIDGETS[i]->name);
      if (NULL != recipe && NULL != recipe->free)
         recipe->free(WIDGETS[i]);

      free(WIDGETS[i]->context);
      free(WIDGETS[i]);
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
