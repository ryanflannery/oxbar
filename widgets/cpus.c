#include <err.h>

#include "cpus.h"

void
wcpus_init(widget_t *w)
{
   settings_t *settings = w->context->settings;
   oxstats_t  *stats    = w->context->stats;
   chart_t   **charts   = w->context->charts;

   const char *colors[] = {
      settings->cpus.chart_color_sys,
      settings->cpus.chart_color_interrupt,
      settings->cpus.chart_color_user,
      settings->cpus.chart_color_nice,
      settings->cpus.chart_color_spin,
      settings->cpus.chart_color_idle
   };

   if (MAX_CHARTS_PER_WIDGET < stats->cpus->ncpu)
      errx(1, "%s: too many cpus (%d)", __FUNCTION__, stats->cpus->ncpu);

   int i = 0;
   for (i = 0; i < stats->cpus->ncpu; i++) {
      charts[i] = chart_init(60, CPUSTATES, true,
            settings->cpus.chart_bgcolor, colors);
   }
}

void
wcpus_free(widget_t *w)
{
   chart_t **charts   = w->context->charts;
   size_t i = 0;
   for (; NULL != charts[i]; i++)
      chart_free(charts[i]);
}

bool
wcpus_enabled(widget_t *w)
{
   return w->context->stats->cpus->is_setup;
}

void
wcpus_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   settings_t *settings = w->context->settings;
   oxstats_t  *stats    = w->context->stats;
   chart_t   **charts   = w->context->charts;

   xdraw_printf(ctx, settings->font.fgcolor, "CPUs: ");

   int i = 0;
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
      xdraw_printf(ctx, settings->font.fgcolor, "% 3.0f%%",
            stats->cpus->cpus[i].percentages[CP_IDLE]);

      if (i != stats->cpus->ncpu - 1) xdraw_printf(ctx, "000000", " ");
   }
}
