#include "battery.h"

bool
wbattery_enabled(widget_t *w)
{
   return w->context->stats->battery->is_setup;
}

void
wbattery_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   settings_t *settings = w->context->settings;
   oxstats_t  *stats    = w->context->stats;

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
