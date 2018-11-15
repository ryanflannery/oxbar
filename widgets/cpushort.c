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
#include "cpushort.h"

bool
wcpushort_enabled(void *wstate)
{
	struct widget_cpu_state *w = wstate;
	return w->stats->cpus.is_setup;
}

void
wcpushort_draw(void *wstate, struct xctx *ctx)
{
	struct widget_cpu_state *w = wstate;
	struct oxstats *stats = w->stats;
	int cpu;
	for (cpu = 0; cpu < stats->cpus.ncpu; cpu++) {
		xdraw_printf(ctx,
			w->settings->fgcolor,
			"CPU%d:% 3.0f%%",
			cpu,
			(100 - stats->cpus.cpus[cpu].percentages[CP_IDLE]));

		if (cpu != stats->cpus.ncpu - 1)
			xdraw_printf(ctx, "000000", " ");
	}
}
