#include <err.h>

#include "widgets.h"

/* battery widget components */
bool  wbattery_enabled(oxstats_t*);
void  wbattery_draw(xctx_t *ctx, void*, settings_t*, oxstats_t*);

/* volume widget components */
bool  wvolume_enabled(oxstats_t*);
void  wvolume_draw(xctx_t *ctx, void*, settings_t*, oxstats_t*);

/* nprocs widget components */
bool  wnprocs_enabled(oxstats_t*);
void  wnprocs_draw(xctx_t *ctx, void*, settings_t*, oxstats_t*);

/* memory widget components */
bool wmemory_enabled(oxstats_t*);
void wmemory_init(settings_t*);
void wmemory_free(void*);
void wmemory_draw(xctx_t *ctx, void*, settings_t*, oxstats_t*);

/* cpu widget components */
bool wcpus_enabled(oxstats_t*);
void wcpus_draw(xctx_t *ctx, void*, settings_t*, oxstats_t*);

/* net widget components */
bool wnet_enabled(oxstats_t*);
void wnet_draw(xctx_t *ctx, void*, settings_t*, oxstats_t*);

/* time widget components */
bool wtime_enabled(oxstats_t*);
void wtime_draw(xctx_t *ctx, void*, settings_t*, oxstats_t*);

/* handy defaults for simple widgets (those that don't need init/free) */
void widget_init_empty(__attribute__((unused)) settings_t* s) { }
#define NO_WIDGET_INIT widget_init_empty
void widget_free_empty(__attribute__((unused)) void *w) { }
#define NO_WIDGET_FREE widget_free_empty

/* build the global list of all available widgets */
widget_t WIDGETS[] = {
   { "battery", wbattery_enabled, NO_WIDGET_INIT, NO_WIDGET_FREE, wbattery_draw, NULL, NULL },
   { "volume",  wvolume_enabled,  NO_WIDGET_INIT, NO_WIDGET_FREE, wvolume_draw,  NULL, NULL },
   { "nprocs",  wnprocs_enabled,  NO_WIDGET_INIT, NO_WIDGET_FREE, wnprocs_draw,  NULL, NULL },
   { "memory",  wmemory_enabled,  NO_WIDGET_INIT, NO_WIDGET_FREE, wmemory_draw,  NULL, NULL },
   { "cpus",    wcpus_enabled,    NO_WIDGET_INIT, NO_WIDGET_FREE, wcpus_draw,    NULL, NULL },
   { "net",     wnet_enabled,     NO_WIDGET_INIT, NO_WIDGET_FREE, wnet_draw,     NULL, NULL },
   { "time",    wtime_enabled,    NO_WIDGET_INIT, NO_WIDGET_FREE, wtime_draw,    NULL, NULL },
};
const size_t NWIDGETS = sizeof(WIDGETS) / sizeof(widget_t);

/* battery */

bool
wbattery_enabled(oxstats_t *stats)
{
   return stats->battery->is_setup;
}

void
wbattery_draw(
      xctx_t     *ctx,
      __attribute__((unused))
      void       *data,
      settings_t *settings,
      oxstats_t  *stats)
{
   xdraw_printf(ctx,
         stats->battery->plugged_in ?
            settings->display.fgcolor :
            settings->battery.fgcolor_unplugged ,
         stats->battery->plugged_in ? "AC " : "BAT ");

   xdraw_progress_bar(ctx,
         settings->battery.chart_bgcolor,
         settings->battery.chart_pgcolor,
         settings->battery.chart_width,
         stats->battery->charge_pct);

   xdraw_printf(ctx,
         settings->display.fgcolor,
         "% 3.0f%%", stats->battery->charge_pct);

   if (-1 != stats->battery->minutes_remaining) {
      xdraw_printf(ctx,
            settings->display.fgcolor,
            " %dh %dm",
            stats->battery->minutes_remaining / 60,
            stats->battery->minutes_remaining % 60);
   }
}

/* volume */
bool
wvolume_enabled(oxstats_t *stats)
{
   return stats->volume->is_setup;
}

void
wvolume_draw(
      xctx_t     *ctx,
      __attribute__((unused))
      void       *data,
      settings_t *settings,
      oxstats_t  *stats)
{
   xdraw_printf(ctx,
         settings->display.fgcolor,
         "Vol: ");

   /* TODO Should volume widget ever handle this case!? I've never had it */
   if (stats->volume->left_pct != stats->volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal",
            __FUNCTION__);

   xdraw_progress_bar(ctx,
         settings->volume.chart_bgcolor,
         settings->volume.chart_pgcolor,
         settings->volume.chart_width,
         stats->volume->left_pct);

   xdraw_printf(ctx,
         settings->display.fgcolor,
         "% 3.0f%%", stats->volume->left_pct);
}

/* nprocs */

bool
wnprocs_enabled(oxstats_t *stats)
{
   return stats->nprocs->is_setup;
}

void
wnprocs_draw(
      xctx_t     *ctx,
      __attribute__((unused))
      void       *data,
      settings_t *settings,
      oxstats_t  *stats)
{
   xdraw_printf(ctx,
      settings->display.fgcolor,
      "#Procs: %d", stats->nprocs->nprocs);
}

/* memory */

bool
wmemory_enabled(oxstats_t *stats)
{
   return stats->memory->is_setup;
}

/*
void
wmemory_init(settings_t *settings)
{
   const char *colors[] = {
      settings->memory.chart_color_active,
      settings->memory.chart_color_total,
      settings->memory.chart_color_free,
   };

   chart_init(60, 3, true, settings->memory.chart_bgcolor, colors);
}

void
wmemory_free(void *data)
{
   chart_free(data);
}
*/

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
wmemory_draw(xctx_t *ctx, __attribute__((unused)) void *data, settings_t *settings, oxstats_t *stats)
{
   static chart_t *chart = NULL;
   if (NULL == chart) {             /* FIRST time setup (only on first call) */
      const char *colors[] = {
         settings->memory.chart_color_active,
         settings->memory.chart_color_total,
         settings->memory.chart_color_free
      };
      chart = chart_init(60, 3, true, settings->memory.chart_bgcolor, colors);
   }

   chart_update(chart, (double[]) {
         stats->memory->active_pct,
         stats->memory->total_pct,
         stats->memory->free_pct
         });

   xdraw_printf(ctx, settings->display.fgcolor, "Mem: ");
   xdraw_chart(ctx, chart);
   xdraw_printf(ctx, settings->memory.chart_color_active, " %s",
         fmt_memory("%.1lf", stats->memory->active));
   xdraw_printf(ctx, settings->display.fgcolor, " act ");
   xdraw_printf(ctx, settings->memory.chart_color_total, "%s",
         fmt_memory("%.1lf", stats->memory->total));
   xdraw_printf(ctx, settings->display.fgcolor, " tot ");
   xdraw_printf(ctx, settings->memory.chart_color_free,
         fmt_memory("%.1lf", stats->memory->free));
   xdraw_printf(ctx, settings->display.fgcolor, " free");
}

bool
wcpus_enabled(oxstats_t *stats)
{
   return stats->cpus->is_setup;
}

void
wcpus_draw(
      xctx_t     *ctx,
      __attribute__((unused))
      void       *data,
      settings_t *settings,
      oxstats_t  *stats)
{
   int i;
   static chart_t **charts = NULL;
   if (NULL == charts) {            /* FIRST time setup (only on first call) */
      const char *colors[] = {
         settings->cpus.chart_color_sys,
         settings->cpus.chart_color_interrupt,
         settings->cpus.chart_color_user,
         settings->cpus.chart_color_nice,
         settings->cpus.chart_color_spin,
         settings->cpus.chart_color_idle
      };

      charts = calloc(stats->cpus->ncpu, sizeof(chart_t*));
      if (NULL == charts)
         err(1, "%s: calloc charts failed", __FUNCTION__);

      for (i = 0; i < stats->cpus->ncpu; i++)
         charts[i] = chart_init(60, CPUSTATES, true,
               settings->cpus.chart_bgcolor, colors);
   }


   xdraw_printf(ctx, settings->display.fgcolor, "CPUs: ");
   for (i = 0; i < stats->cpus->ncpu; i++) {
      chart_update(charts[i], (double[]) {
            stats->cpus->cpus[i].percentages[CP_SYS],
            stats->cpus->cpus[i].percentages[CP_INTR],
            stats->cpus->cpus[i].percentages[CP_USER],
            stats->cpus->cpus[i].percentages[CP_NICE],
            stats->cpus->cpus[i].percentages[CP_SPIN],
            stats->cpus->cpus[i].percentages[CP_IDLE]
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
      xdraw_printf(ctx, settings->display.fgcolor, "% 3.0f%%",
            stats->cpus->cpus[i].percentages[CP_IDLE]);
      if (i != stats->cpus->ncpu - 1) xdraw_printf(ctx, "000000", " ");
   }
}

bool
wnet_enabled(oxstats_t *stats)
{
   return stats->network->is_setup;
}

void
wnet_draw(
      xctx_t     *ctx,
      __attribute__((unused))
      void       *data,
      settings_t *settings,
      oxstats_t  *stats)
{
   static chart_t *chart_in  = NULL;
   static chart_t *chart_out = NULL;
   if (NULL == chart_in || NULL == chart_out) {          /* FIRST time setup */
      const char *colors_in[] = {
         settings->network.inbound_chart_color_pgcolor
      };
      const char *colors_out[] = {
         settings->network.outbound_chart_color_pgcolor
      };
      char *bgcolor_in  = settings->network.inbound_chart_color_bgcolor;
      char *bgcolor_out = settings->network.outbound_chart_color_bgcolor;

      chart_in  = chart_init(60, 1, false, bgcolor_in,  colors_in);
      chart_out = chart_init(60, 1, false, bgcolor_out, colors_out);
   }

   chart_update(chart_in,  (double[]){ stats->network->new_bytes_in });
   chart_update(chart_out, (double[]){ stats->network->new_bytes_out });

   xdraw_printf(ctx, settings->display.fgcolor, "Net: ");
   xdraw_chart(ctx, chart_in);
   xdraw_printf(ctx, "268bd2", " %s ",
         fmt_memory("% .0f", stats->network->new_bytes_in / 1000));
   xdraw_chart(ctx, chart_out);
   xdraw_printf(ctx, "dc322f", " %s",
         fmt_memory("% .0f", stats->network->new_bytes_out / 1000));
}

bool
wtime_enabled(__attribute__((unused)) oxstats_t *stats)
{
   return true;
}

void
wtime_draw(
      xctx_t     *ctx,
      __attribute__((unused))
      void       *data,
      settings_t *settings,
      __attribute__((unused))
      oxstats_t  *stats)
{
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, settings->time.format, localtime(&now));

   xdraw_printf(ctx, settings->display.fgcolor, buffer);
}
