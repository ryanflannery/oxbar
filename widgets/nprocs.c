#include "nprocs.h"

bool
wnprocs_enabled(struct widget *w)
{
   return w->context->stats->nprocs->is_setup;
}

void
wnprocs_draw(struct widget *w, struct xctx *ctx)
{
   xdraw_printf(ctx,
      w->context->settings->font.fgcolor,
      "#Procs: %d", w->context->stats->nprocs->nprocs);
}
