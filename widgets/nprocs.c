#include "nprocs.h"

bool
wnprocs_enabled(widget_t *w)
{
   return w->context->stats->nprocs->is_setup;
}

void
wnprocs_draw(
      widget_t *w,
      xctx_t   *ctx)
{
   xdraw_printf(ctx,
      w->context->settings->font.fgcolor,
      "#Procs: %d", w->context->stats->nprocs->nprocs);
}
