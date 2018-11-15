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

#include <sys/types.h>
#include <sys/sysctl.h>

#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"

static u_int64_t calc_diffs(u_int64_t*, u_int64_t*, u_int64_t*);
static void calc_percents(float*, u_int64_t*, u_int64_t);

void
cpu_init(struct cpu_stats *stats)
{
	size_t size;
	int mib[] = { CTL_HW, HW_NCPUONLINE };

	/* get number of cpu's */
	size = sizeof(stats->ncpu);
	if (sysctl(mib, 2, &(stats->ncpu), &size, NULL, 0) == -1)
		err(1, "HW.NCPU");

	/* allocate array of cpu states */
	stats->cpus = calloc(stats->ncpu, sizeof(struct cpu_states));
	if (NULL == stats->cpus)
		err(1, "calloc failed for %d cpus", stats->ncpu);

	stats->is_setup = true;
	cpu_update(stats);   /* to set initial counters */
}

static u_int64_t
calc_diffs(u_int64_t *new, u_int64_t *old, u_int64_t *diff)
{
	u_int64_t nticks = 0;
	size_t    state;

	for (state = 0; state < CPUSTATES; state++) {
		if (new[state] < old[state])
			diff[state] = INT64_MAX - old[state] + new[state];
		else
			diff[state] = new[state] - old[state];

		nticks += diff[state];
	}

	return nticks;
}

static void
calc_percents(float *percents, u_int64_t *diffs, u_int64_t total)
{
	size_t state;

	if (0 == total)      /* guard divde-by-zero */
		total = 1;

	for (state = 0; state < CPUSTATES; state++) {
		percents[state] = (((float)diffs[state] * 1000.0
		                + ((float)total / 2.0))
		                / (float)total) / 10.0;
	}
}

void
cpu_update(struct cpu_stats *stats)
{
	static int  mib[] = { CTL_KERN, KERN_CPTIME2, 0 };
	int         cpu, state;
	u_int64_t   current[CPUSTATES];   /* current cpu tick counters */
	u_int64_t   diffs[CPUSTATES];     /* diffs from previous counters */
	size_t      size = sizeof(current);

	for (cpu = 0; cpu < stats->ncpu; cpu++) {
		/* update curreent cpu counters */
		mib[2] = cpu;
		if (sysctl(mib, 3, current, &size, NULL, 0) < 0)
			err(1, "KERN.CPTIME2[%d]", cpu);

		u_int64_t  nticks; /* total ticks per cpu */
		nticks = calc_diffs(current, stats->cpus[cpu].raw, diffs);
		calc_percents(stats->cpus[cpu].percentages, diffs, nticks);

		/* copy current back into CPUS state */
		for (state = 0; state < CPUSTATES; state++)
			stats->cpus[cpu].raw[state] = current[state];
	}
}

void
cpu_close(struct cpu_stats *stats)
{
	free(stats->cpus);
}
