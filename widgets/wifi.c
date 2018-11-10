#include <err.h>

#include "util.h"
#include "wifi.h"

bool
wwifi_enabled(void *wstate)
{
   struct generic_wstate *w = wstate;
   return w->stats->wifi->is_setup;
}

void
wwifi_draw(void *wstate, struct xctx *ctx)
{
   struct generic_wstate *w = wstate;
   struct widget_wifi_settings *settings = w->settings;
   xdraw_printf(ctx,
         settings->fgcolor,
         "WIFI: %u%%",
         w->stats->wifi->signal_strength);
}
