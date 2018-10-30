#include <err.h>

#include "nprocs.h"

void *
wnprocs_init(struct oxstats *stats, void *settings)
{
   struct widget_nprocs *w;
   if (NULL == (w = malloc(sizeof(struct widget_nprocs))))
      err(1, "failed to allocate widget_nprocs");

   w->settings = settings;
   w->stats    = stats;
   return w;
}

void
wnprocs_free(void *widget)
{
   free(widget);
}

bool
wnprocs_enabled(void *widget)
{
   struct widget_nprocs *w = widget;
   return w->stats->nprocs->is_setup;
}

void
wnprocs_draw(void *widget, struct xctx *ctx)
{
   struct widget_nprocs *w = widget;
   xdraw_printf(ctx, ctx->xfont->settings->fgcolor,
      "#Procs: %d", w->stats->nprocs->nprocs);
}
