#include <err.h>

#include "util.h"
#include "battery.h"

bool
wbattery_enabled(void *wstate)
{
   struct generic_wstate *w = wstate;
   return w->stats->battery.is_setup;
}

void
wbattery_draw(void *wstate, struct xctx *ctx)
{
   struct generic_wstate *w = wstate;
   struct widget_battery_settings *settings = w->settings;

   xdraw_printf(ctx,
         w->stats->battery.plugged_in ?
            settings->fgcolor :
            settings->fgcolor_unplugged ,
         w->stats->battery.plugged_in ? "AC:" : "BAT:");

   xdraw_progress_bar(ctx,
         settings->chart_bgcolor,
         settings->chart_pgcolor,
         settings->chart_width,
         w->stats->battery.charge_pct);

   xdraw_printf(ctx,
         settings->fgcolor,
         "% 3.0f%%", w->stats->battery.charge_pct);

   if (-1 != w->stats->battery.minutes_remaining) {
      xdraw_printf(ctx,
            settings->fgcolor,
            " %dh %dm",
            w->stats->battery.minutes_remaining / 60,
            w->stats->battery.minutes_remaining % 60);
   }
}
