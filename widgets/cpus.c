#include <err.h>

#include "cpus.h"

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
