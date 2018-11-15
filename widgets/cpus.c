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

#include "cpus.h"

void *
wcpu_init(struct oxstats *stats, void *settings)
{
	struct widget_cpu_state *w;
	if (NULL == (w = malloc(sizeof(struct widget_cpu_state))))
		err(1, "failed to allocate widget_cpu_state");

	w->settings = settings;
	w->stats    = stats;
	w->ncpus    = stats->cpus.ncpu; /* XXX this can change at run-time! */

	const char *colors[] = {
		w->settings->chart_color_system,
		w->settings->chart_color_interrupt,
		w->settings->chart_color_user,
		w->settings->chart_color_nice,
		w->settings->chart_color_spin,
		w->settings->chart_color_idle
	};

	if (NULL == (w->cpu_charts = calloc(w->ncpus, sizeof(struct chart*))))
		err(1, "failed to allocate charts array for cpus");

	size_t cpu;
	for (cpu = 0; cpu < w->ncpus; cpu++) {
		w->cpu_charts[cpu] = chart_init(60, CPUSTATES, true,
		w->settings->chart_bgcolor, colors);
	}

	return w;
}

void
wcpu_free(void *wstate)
{
	struct widget_cpu_state *w = wstate;
	size_t cpu;
	for (cpu = 0; cpu < w->ncpus; cpu++)
		chart_free(w->cpu_charts[cpu]);

	free(w);
}

bool
wcpu_enabled(void *wstate)
{
	struct widget_cpu_state *w = wstate;
	return w->stats->cpus.is_setup;
}

void
wcpu_draw(void *wstate, struct xctx *ctx)
{
	struct widget_cpu_state *w = wstate;
	char *fgcolor = w->settings->fgcolor;
	xdraw_printf(ctx, fgcolor, "CPUs: ");

	size_t cpu;
	for (cpu = 0; cpu < w->ncpus; cpu++) {
		chart_update(w->cpu_charts[cpu], (double[]) {
			w->stats->cpus.cpus[cpu].percentages[CP_SYS],
			w->stats->cpus.cpus[cpu].percentages[CP_INTR],
			w->stats->cpus.cpus[cpu].percentages[CP_USER],
			w->stats->cpus.cpus[cpu].percentages[CP_NICE],
			w->stats->cpus.cpus[cpu].percentages[CP_SPIN],
			w->stats->cpus.cpus[cpu].percentages[CP_IDLE]
		});

		xdraw_chart(ctx, w->cpu_charts[cpu]);
		xdraw_printf(ctx, fgcolor, "%3.0f%%",
		w->stats->cpus.cpus[cpu].percentages[CP_IDLE]);

		if (cpu != w->ncpus - 1)
			xdraw_printf(ctx, "000000", " ");
	}
}
