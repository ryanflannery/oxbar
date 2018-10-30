#include <err.h>

#include "cpuslong.h"

void *
wcpulong_init(struct oxstats *stats, void *settings)
{
   return wcpu_init(stats, settings);
}

void
wcpulong_free(void *widget)
{
   wcpu_free(widget);
}

bool
wcpulong_enabled(void *widget)
{
   return wcpu_enabled(widget);
}

void
wcpulong_draw(void *widget, struct xctx *ctx)
{
   struct widget_cpu *w = widget;
   struct oxstats *stats = w->stats;
   struct widget_cpu_settings *settings = w->settings;
   char *fgcolor = ctx->xfont->settings->fgcolor;

   xdraw_printf(ctx, fgcolor, "CPUs: ");
   size_t cpu;
   for (cpu = 0; cpu < w->ncpus; cpu++) {
      chart_update(w->cpu_charts[cpu], (double[]) {
            stats->cpus->cpus[cpu].percentages[CP_SYS],
            stats->cpus->cpus[cpu].percentages[CP_INTR],
            stats->cpus->cpus[cpu].percentages[CP_USER],
            stats->cpus->cpus[cpu].percentages[CP_NICE],
            stats->cpus->cpus[cpu].percentages[CP_SPIN],
            stats->cpus->cpus[cpu].percentages[CP_IDLE]
            });

      xdraw_chart(ctx, w->cpu_charts[cpu]);
      xdraw_printf(ctx, settings->chart_color_sys,        "% 3.0f%%", stats->cpus->cpus[cpu].percentages[CP_SYS]);
      xdraw_printf(ctx, settings->chart_color_interrupt,  "% 3.0f%%", stats->cpus->cpus[cpu].percentages[CP_INTR]);
      xdraw_printf(ctx, settings->chart_color_user,       "% 3.0f%%", stats->cpus->cpus[cpu].percentages[CP_USER]);
      xdraw_printf(ctx, settings->chart_color_nice,       "% 3.0f%%", stats->cpus->cpus[cpu].percentages[CP_NICE]);
      xdraw_printf(ctx, settings->chart_color_spin,       "% 3.0f%%", stats->cpus->cpus[cpu].percentages[CP_SPIN]);
      xdraw_printf(ctx, settings->chart_color_idle,       "% 3.0f%%", stats->cpus->cpus[cpu].percentages[CP_IDLE]);

      if (cpu != w->ncpus - 1) xdraw_printf(ctx, "000000", " ");
   }
}
