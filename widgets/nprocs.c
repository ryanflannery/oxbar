#include <err.h>

#include "nprocs.h"

void *
wnprocs_init(struct oxstats *stats, __attribute__((unused)) void *settings)
{
   struct widget_nprocs_state *w;
   if (NULL == (w = malloc(sizeof(struct widget_nprocs_state))))
      err(1, "failed to allocate widget_nprocs_state");

   w->stats    = stats;
   return w;
}

void
wnprocs_free(void *wstate)
{
   free(wstate);
}

bool
wnprocs_enabled(void *wstate)
{
   struct widget_nprocs_state *w = wstate;
   return w->stats->nprocs->is_setup;
}

void
wnprocs_draw(void *wstate, struct xctx *ctx)
{
   struct widget_nprocs_state *w = wstate;
   xdraw_printf(ctx, ctx->xfont->settings->fgcolor,
      "#Procs: %d", w->stats->nprocs->nprocs);
}
