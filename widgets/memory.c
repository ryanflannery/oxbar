#include <err.h>

#include "memory.h"
#include "util.h"

void *
wmemory_init(struct oxstats *stats, void *settings)
{
	struct widget_memory_state *w;
	if (NULL == (w = malloc(sizeof(struct widget_memory_state))))
		err(1, "failed to allocate widget_memory_state");

	w->settings = settings;
	w->stats    = stats;

	const char *colors[] = {
		w->settings->chart_color_active,
		w->settings->chart_color_total,
		w->settings->chart_color_free
	};

	w->chart = chart_init(60, 3, true, w->settings->chart_bgcolor, colors);
	return w;
}

void
wmemory_free(void *wstate)
{
	struct widget_memory_state *w = wstate;
	chart_free(w->chart);
	free(w);
}

bool
wmemory_enabled(void *wstate)
{
	struct widget_memory_state *w = wstate;
	return w->stats->memory.is_setup;
}

void
wmemory_draw(void *wstate, struct xctx *ctx)
{
	struct widget_memory_state *w = wstate;
	struct widget_memory_settings *settings = w->settings;
	struct oxstats  *stats    = w->stats;

	chart_update(w->chart, (double[]) {
		stats->memory.active_pct,
		stats->memory.total_pct,
		stats->memory.free_pct
	});

	xdraw_printf(ctx, w->settings->fgcolor, "Mem: ");
	xdraw_chart(ctx, w->chart);
	xdraw_printf(ctx, settings->chart_color_active, " %s",
			fmt_memory("%.1lf", stats->memory.active));
	xdraw_printf(ctx, w->settings->fgcolor, " act ");
	xdraw_printf(ctx, settings->chart_color_total, "%s",
			fmt_memory("%.1lf", stats->memory.total));
	xdraw_printf(ctx, w->settings->fgcolor, " tot ");
	xdraw_printf(ctx, settings->chart_color_free,
			fmt_memory("%.1lf", stats->memory.free));
	xdraw_printf(ctx, w->settings->fgcolor, " free");
}
