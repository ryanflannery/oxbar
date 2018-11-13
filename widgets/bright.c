#include <err.h>

#include "util.h"
#include "bright.h"

bool
wbright_enabled(void *wstate)
{
   struct generic_wstate *w = wstate;
   return w->stats->brightness.is_setup;
}

void
wbright_draw(void *wstate, struct xctx *ctx)
{
   struct generic_wstate *w = wstate;
   struct widget_bright_settings *settings = w->settings;

   xdraw_printf(ctx, settings->fgcolor, "Bright:");

   xdraw_progress_bar(ctx,
         settings->chart_bgcolor,
         settings->chart_pgcolor,
         settings->chart_width,
         w->stats->brightness.brightness);

   xdraw_printf(ctx, settings->fgcolor,
         "% 3.0f%%", w->stats->brightness.brightness);
}
