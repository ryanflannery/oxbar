#include <err.h>

#include "util.h"
#include "nprocs.h"

bool
wnprocs_enabled(void *wstate)
{
	struct generic_wstate *w = wstate;
	return w->stats->nprocs.is_setup;
}

void
wnprocs_draw(void *wstate, struct xctx *ctx)
{
	struct generic_wstate *w = wstate;
	struct widget_nprocs_settings *settings = w->settings;

	xdraw_printf(ctx, settings->fgcolor, "#Procs: %d",
			w->stats->nprocs.nprocs);
}
