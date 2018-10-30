#include <err.h>

#include "cpus.h"

void *
wcpu_init(struct oxstats *stats, void *settings)
{
   struct widget_cpu *w;
   if (NULL == (w = malloc(sizeof(struct widget_cpu))))
      err(1, "failed to allocate widget_cpu");

   w->settings = settings;
   w->stats    = stats;
   w->ncpus    = stats->cpus->ncpu; /* XXX this can change at run-time! */

   const char *colors[] = {
      w->settings->chart_color_sys,
      w->settings->chart_color_interrupt,
      w->settings->chart_color_user,
      w->settings->chart_color_nice,
      w->settings->chart_color_spin,
      w->settings->chart_color_idle
   };

   if (NULL == (w->cpu_charts = calloc(w->ncpus, sizeof(struct chart*))))
      err(1, "failed to allocate charts array for cpus");

   size_t cpu;
   for (cpu = 0; cpu < w->ncpus; cpu++) {
      w->cpu_charts[cpu] = chart_init(60, CPUSTATES, true,
            w->settings->chart_bgcolor, colors);
   }

   return w;
}

void
wcpu_free(void *widget)
{
   struct widget_cpu *w = widget;
   size_t cpu;
   for (cpu = 0; cpu < w->ncpus; cpu++)
      chart_free(w->cpu_charts[cpu]);

   free(w);
}

bool
wcpu_enabled(void *widget)
{
   struct widget_cpu *w = widget;
   return w->stats->cpus->is_setup;
}

void
wcpu_draw(void *widget, struct xctx *ctx)
{
   struct widget_cpu *w = widget;
   char *fgcolor = ctx->xfont->settings->fgcolor;
   xdraw_printf(ctx, fgcolor, "CPUs: ");

   size_t cpu;
   for (cpu = 0; cpu < w->ncpus; cpu++) {
      chart_update(w->cpu_charts[cpu], (double[]) {
            w->stats->cpus->cpus[cpu].percentages[CP_SYS],
            w->stats->cpus->cpus[cpu].percentages[CP_INTR],
            w->stats->cpus->cpus[cpu].percentages[CP_USER],
            w->stats->cpus->cpus[cpu].percentages[CP_NICE],
            w->stats->cpus->cpus[cpu].percentages[CP_SPIN],
            w->stats->cpus->cpus[cpu].percentages[CP_IDLE]
            });

      xdraw_chart(ctx, w->cpu_charts[cpu]);
      xdraw_printf(ctx, fgcolor, "% 3.0f%%",
            w->stats->cpus->cpus[cpu].percentages[CP_IDLE]);

      if (cpu != w->ncpus - 1) xdraw_printf(ctx, "000000", " ");
   }
}
