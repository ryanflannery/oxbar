#include <err.h>
#include <string.h>

#include "gui/xdraw.h"
#include "widgets.h"
#include "widgets/battery.h"
#include "widgets/volume.h"
#include "widgets/nprocs.h"
#include "widgets/memory.h"
#include "widgets/cpus.h"
#include "widgets/net.h"
#include "widgets/time.h"

/*
 * Global list of all known widget types and how to create them.
 * When adding a widget, they must be added here.
 */
typedef struct widget_recipe {
   widget_t widget;              /* the widget itself */
   void (*init)(struct widget*); /* how to build it   */
   void (*free)(struct widget*); /* how to destroy it */
} widget_recipe_t;

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

/* this tracks all widgets created, so they can be destroyed on shutdown */
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

static widget_t*
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

static void
widgets_set_hdcolor(const char *widget_name, char *color)
{
   widget_recipe_t *recipe = find_recipe(widget_name);
   if (NULL == recipe)
      errx(1, "unknown widget '%s'", widget_name);

   recipe->widget.hdcolor = color;
}

static void
widgets_create(
      const char  *list,
      xctx_direction_t direction,
      gui_t      *gui,
      settings_t *settings,
      oxstats_t  *stats)
{
   char *token;
   char *copylist = strdup(list);   /* strsep(3) will change this */
   char *memhandle = copylist;      /* need this for free() and clang */

   if (NULL == copylist)
      err(1, "%s strdup failed", __FUNCTION__);

   while (NULL != (token = strsep(&copylist, " ,"))) {
      if (0 == strlen(token))
            continue;

      gui_add_widget(gui, direction,
            widget_create_from_recipe(token, settings, stats));
   }
   free(memhandle);
}

void
widgets_init(gui_t *gui, settings_t *settings, oxstats_t *stats)
{
   widgets_set_hdcolor("battery",   settings->battery.hdcolor);
   widgets_set_hdcolor("volume",    settings->volume.hdcolor);
   widgets_set_hdcolor("nprocs",    settings->nprocs.hdcolor);
   widgets_set_hdcolor("memory",    settings->memory.hdcolor);
   widgets_set_hdcolor("cpus",      settings->cpus.hdcolor);
   widgets_set_hdcolor("net",       settings->network.hdcolor);
   widgets_set_hdcolor("time",      settings->time.hdcolor);

   widgets_create(settings->display.left,    L2R, gui, settings, stats);
   widgets_create(settings->display.center,  CENTERED, gui, settings, stats);
   widgets_create(settings->display.right,   R2L, gui, settings, stats);
}
