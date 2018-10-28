#include <err.h>

#include "memory.h"
#include "util.h"

void
wmemory_init(struct widget *w)
{
   struct settings *settings = w->context->settings;

   const char *colors[] = {
      settings->memory.chart_color_active,
      settings->memory.chart_color_total,
      settings->memory.chart_color_free
   };

   w->context->charts[0] = chart_init(60, 3, true,
         settings->memory.chart_bgcolor, colors);
}

void
wmemory_free(struct widget *w)
{
   struct chart *c = w->context->charts[0];
   if (NULL != c)
      chart_free(c);
}

bool
wmemory_enabled(struct widget *w)
{
   return w->context->stats->memory->is_setup;
}

void
wmemory_draw(struct widget *w, struct xctx *ctx)
{
   struct settings *settings = w->context->settings;
   struct oxstats  *stats    = w->context->stats;
   struct chart    *chart    = w->context->charts[0];

   chart_update(chart, (double[]) {
         stats->memory->active_pct,
         stats->memory->total_pct,
         stats->memory->free_pct
         });

   xdraw_printf(ctx, settings->font.fgcolor, "Mem: ");
   xdraw_chart(ctx, chart);
   xdraw_printf(ctx, settings->memory.chart_color_active, " %s",
         fmt_memory("%.1lf", stats->memory->active));
   xdraw_printf(ctx, settings->font.fgcolor, " act ");
   xdraw_printf(ctx, settings->memory.chart_color_total, "%s",
         fmt_memory("%.1lf", stats->memory->total));
   xdraw_printf(ctx, settings->font.fgcolor, " tot ");
   xdraw_printf(ctx, settings->memory.chart_color_free,
         fmt_memory("%.1lf", stats->memory->free));
   xdraw_printf(ctx, settings->font.fgcolor, " free");
}
