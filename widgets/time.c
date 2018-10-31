#include <err.h>
#include <time.h>

#include "time.h"

void *
wtime_init(struct oxstats *stats, void *settings)
{
   struct widget_time_state *w;
   if (NULL == (w = malloc(sizeof(struct widget_time_state))))
      err(1, "failed to allocate widget_time_state");

   w->settings = settings;
   w->stats    = stats;
   return w;
}

void
wtime_free(void *wstate)
{
   free(wstate);
}

bool
wtime_enabled(__attribute__((unused)) void *wstate)
{
   return true;
}

void
wtime_draw(void *wstate, struct xctx *ctx)
{
   struct widget_time_state *w = wstate;
#define GUI_TIME_MAXLEN 100
   static char buffer[GUI_TIME_MAXLEN];

   time_t now = time(NULL);
   strftime(buffer, GUI_TIME_MAXLEN, w->settings->format, localtime(&now));
   xdraw_printf(ctx, w->settings->fgcolor, buffer);
}
