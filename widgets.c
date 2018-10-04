#include <err.h>

#include "widgets.h"
#include "gui/xdraw.h"

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

   w->name = recipe->name;
   w->enabled = recipe->enabled;
   w->init = recipe->init;
   w->free = recipe->free;
   w->draw = recipe->draw;
   w->settings = settings;
   w->stats    = stats;
   w->hdcolor  = recipe->hdcolor;

   return w;
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

/* individual widget stuff below */

/* battery widget components */
bool  wbattery_enabled(widget_t*);
void  wbattery_draw(widget_t*, xctx_t *ctx);

/* volume widget components */
bool  wvolume_enabled(widget_t*);
void  wvolume_draw(widget_t*, xctx_t *ctx);

/* nprocs widget components */
bool  wnprocs_enabled(widget_t*);
void  wnprocs_draw(widget_t*, xctx_t *ctx);

/* memory widget components */
bool wmemory_enabled(widget_t*);
void wmemory_draw(widget_t*, xctx_t *ctx);

/* cpu widget components */
bool wcpus_enabled(widget_t*);
void wcpus_draw(widget_t*, xctx_t *ctx);

/* net widget components */
bool wnet_enabled(widget_t*);
void wnet_draw(widget_t*, xctx_t *ctx);

/* time widget components */
bool wtime_enabled(widget_t*);
void wtime_draw(widget_t*, xctx_t *ctx);

/* handy defaults for simple widgets (those that don't need init/free) */
void widget_init_empty(__attribute__((unused)) settings_t *s) { }
#define NO_WIDGET_INIT widget_init_empty
void widget_free_empty(__attribute__((unused)) widget_t *w) { }
#define NO_WIDGET_FREE widget_free_empty

/* build the global list of all available widgets */
widget_t WIDGET_RECIPES[] = {
   { "battery", wbattery_enabled, NO_WIDGET_INIT, NO_WIDGET_FREE, wbattery_draw, NULL, NULL, NULL, NULL },
   { "volume",  wvolume_enabled,  NO_WIDGET_INIT, NO_WIDGET_FREE, wvolume_draw,  NULL, NULL, NULL, NULL },
   { "nprocs",  wnprocs_enabled,  NO_WIDGET_INIT, NO_WIDGET_FREE, wnprocs_draw,  NULL, NULL, NULL, NULL },
   { "memory",  wmemory_enabled,  NO_WIDGET_INIT, NO_WIDGET_FREE, wmemory_draw,  NULL, NULL, NULL, NULL },
   { "cpus",    wcpus_enabled,    NO_WIDGET_INIT, NO_WIDGET_FREE, wcpus_draw,    NULL, NULL, NULL, NULL },
   { "net",     wnet_enabled,     NO_WIDGET_INIT, NO_WIDGET_FREE, wnet_draw,     NULL, NULL, NULL, NULL },
   { "time",    wtime_enabled,    NO_WIDGET_INIT, NO_WIDGET_FREE, wtime_draw,    NULL, NULL, NULL, NULL },
};
const size_t NWIDGET_RECIPES = sizeof(WIDGET_RECIPES) / sizeof(widget_t);

/* battery */

bool
wbattery_enabled(widget_t *w)
{
   return w->stats->battery->is_setup;
}

void
wbattery_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   xdraw_printf(ctx,
         w->stats->battery->plugged_in ?
            w->settings->display.fgcolor :
            w->settings->battery.fgcolor_unplugged ,
         w->stats->battery->plugged_in ? "AC " : "BAT ");

   xdraw_progress_bar(ctx,
         w->settings->battery.chart_bgcolor,
         w->settings->battery.chart_pgcolor,
         w->settings->battery.chart_width,
         w->stats->battery->charge_pct);

   xdraw_printf(ctx,
         w->settings->display.fgcolor,
         "% 3.0f%%", w->stats->battery->charge_pct);

   if (-1 != w->stats->battery->minutes_remaining) {
      xdraw_printf(ctx,
            w->settings->display.fgcolor,
            " %dh %dm",
            w->stats->battery->minutes_remaining / 60,
            w->stats->battery->minutes_remaining % 60);
   }
}

/* volume */
bool
wvolume_enabled(widget_t *w)
{
   return w->stats->volume->is_setup;
}

void
wvolume_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   xdraw_printf(ctx,
         w->settings->display.fgcolor,
         "Vol: ");

   /* TODO Should volume widget ever handle this case!? I've never had it */
   if (w->stats->volume->left_pct != w->stats->volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal",
            __FUNCTION__);

   xdraw_progress_bar(ctx,
         w->settings->volume.chart_bgcolor,
         w->settings->volume.chart_pgcolor,
         w->settings->volume.chart_width,
         w->stats->volume->left_pct);

   xdraw_printf(ctx,
         w->settings->display.fgcolor,
         "% 3.0f%%", w->stats->volume->left_pct);
}

/* nprocs */

bool
wnprocs_enabled(widget_t *w)
{
   return w->stats->nprocs->is_setup;
}

void
wnprocs_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   xdraw_printf(ctx,
      w->settings->display.fgcolor,
      "#Procs: %d", w->stats->nprocs->nprocs);
}

/* memory */

bool
wmemory_enabled(widget_t *w)
{
   return w->stats->memory->is_setup;
}

static const char *
fmt_memory(const char *fmt, int kbytes)
{
#define GUI_FMT_MEMORY_BUFFER_MAXLEN 100
   static char          buffer[GUI_FMT_MEMORY_BUFFER_MAXLEN];
   static const char   *suffixes[] = { "k", "M", "G", "T", "P" };
   static const size_t  snum       = sizeof(suffixes) / sizeof(suffixes[0]);

   double dbytes = (double) kbytes;
   size_t step   = 0;
   int    ret;

   if (1024 < kbytes)
      for (step = 0; (kbytes / 1024) > 0 && step < snum; step++, kbytes /= 1024)
         dbytes = kbytes / 1024.0;

   ret = snprintf(buffer, GUI_FMT_MEMORY_BUFFER_MAXLEN, fmt, dbytes);
   if (0 > ret)
      err(1, "%s: snprintf failed for %d", __FUNCTION__, kbytes);

   ret = snprintf(buffer + ret, GUI_FMT_MEMORY_BUFFER_MAXLEN- ret, "%s",
         suffixes[step]);
   if (0 > ret)
      err(1, "%s: snprintf failed for %s", __FUNCTION__, suffixes[step]);

   return buffer;
}

void
wmemory_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   static chart_t *chart = NULL;
   if (NULL == chart) {             /* FIRST time setup (only on first call) */
      const char *colors[] = {
         w->settings->memory.chart_color_active,
         w->settings->memory.chart_color_total,
         w->settings->memory.chart_color_free
      };
      chart = chart_init(60, 3, true, w->settings->memory.chart_bgcolor, colors);
   }

   chart_update(chart, (double[]) {
         w->stats->memory->active_pct,
         w->stats->memory->total_pct,
         w->stats->memory->free_pct
         });

   xdraw_printf(ctx, w->settings->display.fgcolor, "Mem: ");
   xdraw_chart(ctx, chart);
   xdraw_printf(ctx, w->settings->memory.chart_color_active, " %s",
         fmt_memory("%.1lf", w->stats->memory->active));
   xdraw_printf(ctx, w->settings->display.fgcolor, " act ");
   xdraw_printf(ctx, w->settings->memory.chart_color_total, "%s",
         fmt_memory("%.1lf", w->stats->memory->total));
   xdraw_printf(ctx, w->settings->display.fgcolor, " tot ");
   xdraw_printf(ctx, w->settings->memory.chart_color_free,
         fmt_memory("%.1lf", w->stats->memory->free));
   xdraw_printf(ctx, w->settings->display.fgcolor, " free");
}

bool
wcpus_enabled(widget_t *w)
{
   return w->stats->cpus->is_setup;
}

void
wcpus_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   int i;
   static chart_t **charts = NULL;
   if (NULL == charts) {            /* FIRST time setup (only on first call) */
      const char *colors[] = {
         w->settings->cpus.chart_color_sys,
         w->settings->cpus.chart_color_interrupt,
         w->settings->cpus.chart_color_user,
         w->settings->cpus.chart_color_nice,
         w->settings->cpus.chart_color_spin,
         w->settings->cpus.chart_color_idle
      };

      charts = calloc(w->stats->cpus->ncpu, sizeof(chart_t*));
      if (NULL == charts)
         err(1, "%s: calloc charts failed", __FUNCTION__);

      for (i = 0; i < w->stats->cpus->ncpu; i++)
         charts[i] = chart_init(60, CPUSTATES, true,
               w->settings->cpus.chart_bgcolor, colors);
   }


   xdraw_printf(ctx, w->settings->display.fgcolor, "CPUs: ");
   for (i = 0; i < w->stats->cpus->ncpu; i++) {
      chart_update(charts[i], (double[]) {
            w->stats->cpus->cpus[i].percentages[CP_SYS],
            w->stats->cpus->cpus[i].percentages[CP_INTR],
            w->stats->cpus->cpus[i].percentages[CP_USER],
            w->stats->cpus->cpus[i].percentages[CP_NICE],
            w->stats->cpus->cpus[i].percentages[CP_SPIN],
            w->stats->cpus->cpus[i].percentages[CP_IDLE]
            });
      xdraw_chart(ctx, charts[i]);
      /* TODO revisit "cpulong" widget printing individual states
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_SYS]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_INTR]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_USER]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_NICE]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_SPIN]);
      xdraw_printf(context, settings->display.fgcolor, "% .0f%%", stats->cpus->cpus[i].percentages[CP_IDLE]);
      */
      xdraw_printf(ctx, w->settings->display.fgcolor, "% 3.0f%%",
            w->stats->cpus->cpus[i].percentages[CP_IDLE]);
      if (i != w->stats->cpus->ncpu - 1) xdraw_printf(ctx, "000000", " ");
   }
}

bool
wnet_enabled(widget_t *w)
{
   return w->stats->network->is_setup;
}

void
wnet_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   static chart_t *chart_in  = NULL;
   static chart_t *chart_out = NULL;
   if (NULL == chart_in || NULL == chart_out) {          /* FIRST time setup */
      const char *colors_in[] = {
         w->settings->network.inbound_chart_color_pgcolor
      };
      const char *colors_out[] = {
         w->settings->network.outbound_chart_color_pgcolor
      };
      char *bgcolor_in  = w->settings->network.inbound_chart_color_bgcolor;
      char *bgcolor_out = w->settings->network.outbound_chart_color_bgcolor;

      chart_in  = chart_init(60, 1, false, bgcolor_in,  colors_in);
      chart_out = chart_init(60, 1, false, bgcolor_out, colors_out);
   }

   chart_update(chart_in,  (double[]){ w->stats->network->new_bytes_in });
   chart_update(chart_out, (double[]){ w->stats->network->new_bytes_out });

   xdraw_printf(ctx, w->settings->display.fgcolor, "Net: ");
   xdraw_chart(ctx, chart_in);
   xdraw_printf(ctx, "268bd2", " %s ",
         fmt_memory("% .0f", w->stats->network->new_bytes_in / 1000));
   xdraw_chart(ctx, chart_out);
   xdraw_printf(ctx, "dc322f", " %s",
         fmt_memory("% .0f", w->stats->network->new_bytes_out / 1000));
}

bool
wtime_enabled(__attribute__((unused)) widget_t *w)
{
   return true;
}

void
wtime_draw(
      widget_t *w,
      xctx_t   *ctx)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, w->settings->time.format, localtime(&now));

   xdraw_printf(ctx, w->settings->display.fgcolor, buffer);
}
