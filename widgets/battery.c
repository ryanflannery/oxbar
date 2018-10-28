#include "battery.h"

bool
wbattery_enabled(struct widget *w)
{
   return w->context->stats->battery->is_setup;
}

void
wbattery_draw(struct widget *w, struct xctx *ctx)
{
   struct settings *settings = w->context->settings;
   struct oxstats  *stats    = w->context->stats;

   xdraw_printf(ctx,
         stats->battery->plugged_in ?
            settings->font.fgcolor :
            settings->battery.fgcolor_unplugged ,
         stats->battery->plugged_in ? "AC " : "BAT ");

   xdraw_progress_bar(ctx,
         settings->battery.chart_bgcolor,
         settings->battery.chart_pgcolor,
         settings->battery.chart_width,
         stats->battery->charge_pct);

   xdraw_printf(ctx,
         settings->font.fgcolor,
         "% 3.0f%%", stats->battery->charge_pct);

   if (-1 != stats->battery->minutes_remaining) {
      xdraw_printf(ctx,
            settings->font.fgcolor,
            " %dh %dm",
            stats->battery->minutes_remaining / 60,
            stats->battery->minutes_remaining % 60);
   }
}
