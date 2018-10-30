#include <err.h>

#include "cpus.h"
#include "cpushort.h"

void *
wcpushort_init(struct oxstats *stats, void *settings)
{
   return wcpu_init(stats, settings);
}

void
wcpushort_free(void *wstate)
{
   wcpu_free(wstate);
}

bool
wcpushort_enabled(void *wstate)
{
   struct widget_cpu_state *w = wstate;
   return w->stats->cpus->is_setup;
}

void
wcpushort_draw(void *wstate, struct xctx *ctx)
{
   struct widget_cpu_state *w = wstate;
   struct oxstats *stats = w->stats;
   int cpu;
   for (cpu = 0; cpu < stats->cpus->ncpu; cpu++) {
      xdraw_printf(ctx, ctx->xfont->settings->fgcolor, "CPU%d: ", cpu);
      xdraw_printf(ctx, ctx->xfont->settings->fgcolor, "% 3.0f%%",
            (100 - stats->cpus->cpus[cpu].percentages[CP_IDLE]));

      if (cpu != stats->cpus->ncpu - 1) xdraw_printf(ctx, "000000", " ");
   }
}
