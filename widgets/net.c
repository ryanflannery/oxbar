#include <err.h>

#include "net.h"
#include "util.h"

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
   settings_t *settings = w->context->settings;
   oxstats_t  *stats    = w->context->stats;

   static chart_t *chart_in  = NULL;
   static chart_t *chart_out = NULL;
   if (NULL == chart_in || NULL == chart_out) {          /* FIRST time setup */
      const char *colors_in[] = {
         settings->network.inbound_chart_color_pgcolor
      };
      const char *colors_out[] = {
         settings->network.outbound_chart_color_pgcolor
      };
      char *bgcolor_in  = settings->network.inbound_chart_color_bgcolor;
      char *bgcolor_out = settings->network.outbound_chart_color_bgcolor;

      chart_in  = chart_init(60, 1, false, bgcolor_in,  colors_in);
      chart_out = chart_init(60, 1, false, bgcolor_out, colors_out);
   }

   chart_update(chart_in,  (double[]){ stats->network->new_bytes_in });
   chart_update(chart_out, (double[]){ stats->network->new_bytes_out });

   xdraw_printf(ctx, settings->display.fgcolor, "Net: ");
   xdraw_chart(ctx, chart_in);
   xdraw_printf(ctx, "268bd2", " %s ",
         fmt_memory("% .0f", stats->network->new_bytes_in / 1000));
   xdraw_chart(ctx, chart_out);
   xdraw_printf(ctx, "dc322f", " %s",
         fmt_memory("% .0f", stats->network->new_bytes_out / 1000));
}
