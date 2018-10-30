#include <err.h>

#include "battery.h"

void *
wbattery_init(struct oxstats *stats, void *settings)
{
   struct widget_battery_state *w;
   if (NULL == (w = malloc(sizeof(struct widget_battery_state))))
      err(1, "failed to allocate widget_battery_state");

   w->settings = settings;
   w->stats    = stats;
   return w;
}

void
wbattery_free(void *wstate)
{
   free(wstate);
}

bool
wbattery_enabled(void *wstate)
{
   struct widget_battery_state *w = wstate;
   return w->stats->battery->is_setup;
}

void
wbattery_draw(void *wstate, struct xctx *ctx)
{
   struct widget_battery_state *w = wstate;

   xdraw_printf(ctx,
         w->stats->battery->plugged_in ?
            ctx->xfont->settings->fgcolor :
            w->settings->fgcolor_unplugged ,
         w->stats->battery->plugged_in ? "AC " : "BAT ");

   xdraw_progress_bar(ctx,
         w->settings->chart_bgcolor,
         w->settings->chart_pgcolor,
         w->settings->chart_width,
         w->stats->battery->charge_pct);

   xdraw_printf(ctx,
         ctx->xfont->settings->fgcolor,
         "% 3.0f%%", w->stats->battery->charge_pct);

   if (-1 != w->stats->battery->minutes_remaining) {
      xdraw_printf(ctx,
            ctx->xfont->settings->fgcolor,
            " %dh %dm",
            w->stats->battery->minutes_remaining / 60,
            w->stats->battery->minutes_remaining % 60);
   }
}
