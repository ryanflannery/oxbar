#include <err.h>

#include "cpushort.h"

bool
wcpushort_enabled(struct widget *w)
{
   return w->context->stats->cpus->is_setup;
}

void
wcpushort_draw(struct widget *w, struct xctx *ctx)
{
   struct settings *settings = w->context->settings;
   struct oxstats  *stats    = w->context->stats;

   int i = 0;
   for (i = 0; i < stats->cpus->ncpu; i++) {
      xdraw_printf(ctx, settings->font.fgcolor, "CPU%d: ", i);
      xdraw_printf(ctx, settings->font.fgcolor, "% 3.0f%%",
            (100 - stats->cpus->cpus[i].percentages[CP_IDLE]));

      if (i != stats->cpus->ncpu - 1) xdraw_printf(ctx, "000000", " ");
   }
}
