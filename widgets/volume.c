#include <err.h>

#include "volume.h"

bool
wvolume_enabled(widget_t *w)
{
   return w->context->stats->volume->is_setup;
}

void
wvolume_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   settings_t *settings = w->context->settings;
   oxstats_t  *stats    = w->context->stats;

   xdraw_printf(ctx, settings->display.fgcolor, "Vol: ");

   /* TODO Should volume widget ever handle this case!? I've never had it */
   if (stats->volume->left_pct != stats->volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal",
            __FUNCTION__);

   xdraw_progress_bar(ctx,
         settings->volume.chart_bgcolor,
         settings->volume.chart_pgcolor,
         settings->volume.chart_width,
         stats->volume->left_pct);

   xdraw_printf(ctx,
         settings->display.fgcolor,
         "% 3.0f%%", stats->volume->left_pct);
}
