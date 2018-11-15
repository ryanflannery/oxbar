/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>

#include "cpuslong.h"

bool
wcpulong_enabled(void *wstate)
{
	return wcpu_enabled(wstate);
}

void
wcpulong_draw(void *wstate, struct xctx *ctx)
{
	struct widget_cpu_state *w = wstate;
	struct oxstats *stats = w->stats;
	struct widget_cpu_settings *settings = w->settings;
	char *fgcolor = w->settings->fgcolor;

	xdraw_printf(ctx, fgcolor, "CPUs: ");
	size_t cpu;
	for (cpu = 0; cpu < w->ncpus; cpu++) {
		chart_update(w->cpu_charts[cpu], (double[]) {
			stats->cpus.cpus[cpu].percentages[CP_SYS],
			stats->cpus.cpus[cpu].percentages[CP_INTR],
			stats->cpus.cpus[cpu].percentages[CP_USER],
			stats->cpus.cpus[cpu].percentages[CP_NICE],
			stats->cpus.cpus[cpu].percentages[CP_SPIN],
			stats->cpus.cpus[cpu].percentages[CP_IDLE]
		});

		xdraw_chart(ctx, w->cpu_charts[cpu]);
		xdraw_printf(ctx, settings->chart_color_system,     "%3.0f%%",
				stats->cpus.cpus[cpu].percentages[CP_SYS]);
		xdraw_printf(ctx, settings->chart_color_interrupt,  "%3.0f%%",
				stats->cpus.cpus[cpu].percentages[CP_INTR]);
		xdraw_printf(ctx, settings->chart_color_user,       "%3.0f%%",
				stats->cpus.cpus[cpu].percentages[CP_USER]);
		xdraw_printf(ctx, settings->chart_color_nice,       "%3.0f%%",
				stats->cpus.cpus[cpu].percentages[CP_NICE]);
		xdraw_printf(ctx, settings->chart_color_spin,       "%3.0f%%",
				stats->cpus.cpus[cpu].percentages[CP_SPIN]);
		xdraw_printf(ctx, settings->chart_color_idle,       "%3.0f%%",
				stats->cpus.cpus[cpu].percentages[CP_IDLE]);

		if (cpu != w->ncpus - 1) xdraw_printf(ctx, "000000", " ");
	}
}
