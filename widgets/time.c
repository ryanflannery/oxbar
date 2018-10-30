#include <err.h>
#include <time.h>

#include "time.h"

void *
wtime_init(struct oxstats *stats, void *settings)
{
   struct widget_time *w;
   if (NULL == (w = malloc(sizeof(struct widget_time))))
      err(1, "failed to allocate widget_time");

   w->settings = settings;
   w->stats    = stats;
   return w;
}

void
wtime_free(void *widget)
{
   free(widget);
}

bool
wtime_enabled(__attribute__((unused)) void *widget)
{
   return true;
}

void
wtime_draw(void *widget, struct xctx *ctx)
{
   struct widget_time *w = widget;
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, w->settings->format, localtime(&now));
   xdraw_printf(ctx, ctx->xfont->settings->fgcolor, buffer);
}
