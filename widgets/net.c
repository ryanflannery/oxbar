#include <err.h>

#include "net.h"
#include "util.h"

void
wnet_init(widget_t *w)
{
   settings_t *settings = w->context->settings;
   chart_t   **charts   = w->context->charts;

   const char *colors_in[] = {
      settings->network.inbound_chart_color_pgcolor
   };
   const char *colors_out[] = {
      settings->network.outbound_chart_color_pgcolor
   };
   char *bgcolor_in  = settings->network.inbound_chart_color_bgcolor;
   char *bgcolor_out = settings->network.outbound_chart_color_bgcolor;

   charts[0] = chart_init(60, 1, false, bgcolor_in,  colors_in);
   charts[1] = chart_init(60, 1, false, bgcolor_out, colors_out);
}

void
wnet_free(widget_t *w)
{
   chart_t **charts = w->context->charts;
   chart_free(charts[0]);
   chart_free(charts[1]);
}

bool
wnet_enabled(widget_t *w)
{
   return w->context->stats->network->is_setup;
}

void
wnet_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   settings_t *settings  = w->context->settings;
   oxstats_t  *stats     = w->context->stats;
   chart_t    *chart_in  = w->context->charts[0];
   chart_t    *chart_out = w->context->charts[1];;

   chart_update(chart_in,  (double[]){ stats->network->new_bytes_in });
   chart_update(chart_out, (double[]){ stats->network->new_bytes_out });

   xdraw_printf(ctx, settings->fgcolor, "Net: ");
   xdraw_chart(ctx, chart_in);
   xdraw_printf(ctx, "268bd2", " %s ",
         fmt_memory("% .0f", stats->network->new_bytes_in / 1000));
   xdraw_chart(ctx, chart_out);
   xdraw_printf(ctx, "dc322f", " %s",
         fmt_memory("% .0f", stats->network->new_bytes_out / 1000));
}
