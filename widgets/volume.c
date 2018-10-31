#include <err.h>

#include "volume.h"

void *
wvolume_init(struct oxstats *stats, void *settings)
{
   struct widget_volume_state *w;
   if (NULL == (w = malloc(sizeof(struct widget_volume_state))))
      err(1, "failed to allocate widget_volume_state");

   w->settings = settings;
   w->stats    = stats;
   return w;
}

void
wvolume_free(void *wstate)
{
   free(wstate);
}

bool
wvolume_enabled(void *wstate)
{
   struct widget_volume_state *w = wstate;
   return w->stats->volume->is_setup;
}

void
wvolume_draw(void *wstate, struct xctx *ctx)
{
   struct widget_volume_state *w = wstate;
   struct widget_volume_settings *settings = w->settings;
   struct oxstats *stats = w->stats;

   xdraw_printf(ctx, w->settings->fgcolor, "Vol: ");

   /* TODO Should volume widget ever handle this case!? I've never had it */
   if (stats->volume->left_pct != stats->volume->right_pct)
      warnx("%s: left & right volume aren't properly rendered if not equal",
            __FUNCTION__);

   xdraw_progress_bar(ctx,
         settings->chart_bgcolor,
         settings->chart_pgcolor,
         settings->chart_width,
         stats->volume->left_pct);

   xdraw_printf(ctx, w->settings->fgcolor,
         "% 3.0f%%", stats->volume->left_pct);
}
