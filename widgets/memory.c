#include <err.h>

#include "memory.h"
#include "util.h"

void
wmemory_init(widget_t *w)
{
   settings_t *settings = w->context->settings;

   const char *colors[] = {
      settings->memory.chart_color_active,
      settings->memory.chart_color_total,
      settings->memory.chart_color_free
   };

   w->context->charts[0] = chart_init(60, 3, true,
         settings->memory.chart_bgcolor, colors);
}

void
wmemory_free(widget_t *w)
{
   chart_t *c = w->context->charts[0];
   if (NULL != c)
      chart_free(c);
}

bool
wmemory_enabled(widget_t *w)
{
   return w->context->stats->memory->is_setup;
}

void
wmemory_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   settings_t *settings = w->context->settings;
   oxstats_t  *stats    = w->context->stats;
   chart_t    *chart    = w->context->charts[0];

   chart_update(chart, (double[]) {
         stats->memory->active_pct,
         stats->memory->total_pct,
         stats->memory->free_pct
         });

   xdraw_printf(ctx, settings->fgcolor, "Mem: ");
   xdraw_chart(ctx, chart);
   xdraw_printf(ctx, settings->memory.chart_color_active, " %s",
         fmt_memory("%.1lf", stats->memory->active));
   xdraw_printf(ctx, settings->fgcolor, " act ");
   xdraw_printf(ctx, settings->memory.chart_color_total, "%s",
         fmt_memory("%.1lf", stats->memory->total));
   xdraw_printf(ctx, settings->fgcolor, " tot ");
   xdraw_printf(ctx, settings->memory.chart_color_free,
         fmt_memory("%.1lf", stats->memory->free));
   xdraw_printf(ctx, settings->fgcolor, " free");
}
