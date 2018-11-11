#include <err.h>

#include "util.h"
#include "volume.h"

bool
wvolume_enabled(void *wstate)
{
   struct generic_wstate *w = wstate;
   return w->stats->volume->is_setup;
}

void
wvolume_draw(void *wstate, struct xctx *ctx)
{
   struct generic_wstate *w = wstate;
   struct widget_volume_settings *settings = w->settings;
   struct oxstats *stats = w->stats;

   xdraw_printf(ctx, settings->fgcolor, "Vol:");

   if (stats->volume->left_pct == stats->volume->right_pct) {
      xdraw_progress_bar(ctx,
            settings->chart_bgcolor,
            settings->chart_pgcolor,
            settings->chart_width,
            stats->volume->left_pct);
      xdraw_printf(ctx, settings->fgcolor,
            "% 3.0f%%", stats->volume->left_pct);
   } else {
      xdraw_printf(ctx, settings->fgcolor,
            "%3.0f%% ", stats->volume->left_pct);

      xdraw_progress_bar(ctx,
            settings->chart_bgcolor,
            settings->chart_pgcolor,
            settings->chart_width / 2 + 1,
            stats->volume->left_pct);

      xctx_advance(ctx, AFTER_RENDER, 2, 0); /* 2-pixel space between sides */

      xdraw_progress_bar(ctx,
            settings->chart_bgcolor,
            settings->chart_pgcolor,
            settings->chart_width / 2 + 1,
            stats->volume->right_pct);

      xdraw_printf(ctx, settings->fgcolor,
            "% 3.0f%%", stats->volume->right_pct);
   }
}
