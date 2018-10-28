#include <err.h>

#include "cpushort.h"

bool
wcpushort_enabled(widget_t *w)
{
   return w->context->stats->cpus->is_setup;
}

void
wcpushort_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   settings_t *settings = w->context->settings;
   oxstats_t  *stats    = w->context->stats;

   int i = 0;
   for (i = 0; i < stats->cpus->ncpu; i++) {
      xdraw_printf(ctx, settings->font.fgcolor, "CPU%d: ", i);
      xdraw_printf(ctx, settings->font.fgcolor, "% 3.0f%%",
            (100 - stats->cpus->cpus[i].percentages[CP_IDLE]));

      if (i != stats->cpus->ncpu - 1) xdraw_printf(ctx, "000000", " ");
   }
}
