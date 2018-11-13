#include <err.h>

#include "net.h"
#include "util.h"

void *
wnet_init(struct oxstats *stats, void *settings)
{
   struct widget_net_state *w;
   if (NULL == (w = malloc(sizeof(struct widget_net_state))))
      err(1, "failed to allocate widget_net_state");

   w->settings = settings;
   w->stats    = stats;

   const char *colors_in[] = {
      w->settings->inbound_chart_color_pgcolor
   };
   const char *colors_out[] = {
      w->settings->outbound_chart_color_pgcolor
   };
   char *bgcolor_in  = w->settings->inbound_chart_color_bgcolor;
   char *bgcolor_out = w->settings->outbound_chart_color_bgcolor;

   w->inbound  = chart_init(60, 1, false, bgcolor_in,  colors_in);
   w->outbound = chart_init(60, 1, false, bgcolor_out, colors_out);
   return w;
}

void
wnet_free(void *wstate)
{
   struct widget_net_state *w = wstate;
   chart_free(w->inbound);
   chart_free(w->outbound);
   free(w);
}

bool
wnet_enabled(void *wstate)
{
   struct widget_net_state *w = wstate;
   return w->stats->network.is_setup;
}

void
wnet_draw(void *wstate, struct xctx *ctx)
{
   struct widget_net_state *w = wstate;
   struct oxstats  *stats     = w->stats;
   struct chart    *chart_in  = w->inbound;
   struct chart    *chart_out = w->outbound;

   chart_update(chart_in,  (double[]){ stats->network.new_bytes_in });
   chart_update(chart_out, (double[]){ stats->network.new_bytes_out });

   xdraw_printf(ctx, w->settings->fgcolor, "Net: ");
   xdraw_chart(ctx, chart_in);
   xdraw_printf(ctx, w->settings->inbound_text_fgcolor, " %4s ",
         fmt_memory("% .0f", stats->network.new_bytes_in / 1000));
   xdraw_chart(ctx, chart_out);
   xdraw_printf(ctx, w->settings->outbound_text_fgcolor, " %4s",
         fmt_memory("% .0f", stats->network.new_bytes_out / 1000));
}
