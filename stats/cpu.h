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

#ifndef CORE_H
#define CORE_H

#include <sched.h>
#include <sys/sched.h>

#include <stdbool.h>

/* XXX see sys/sched.h for definition of CPUSTATES */
struct cpu_states {
	u_int64_t   raw[CPUSTATES];            /* raw tick counters for each */
	float       percentages[CPUSTATES];    /* percent time spent in each */
};

struct cpu_stats {
	bool               is_setup;
	int                ncpu;      /* number of CPUs available */
	struct cpu_states *cpus;      /* array of cpu states, one for each cpu */
};

void cpu_init(struct cpu_stats*);
void cpu_update(struct cpu_stats*);
void cpu_close(struct cpu_stats*);

#endif
