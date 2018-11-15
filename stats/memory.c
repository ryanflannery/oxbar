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

#include <sys/param.h>
#include <sys/swap.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <uvm/uvmexp.h>

#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "memory.h"

static int sys_pageshift;

void
memory_init(struct memory_stats *stats)
{
	int pgsize;

	/* determine page-shift, needed to accurately interpret vminfo values */
	pgsize = getpagesize();
	sys_pageshift = 0;
	while (pgsize > 1) {
		sys_pageshift++;
		pgsize >>= 1;
	}
	sys_pageshift -= 10;

	stats->is_setup = true;
}

void
memory_update(struct memory_stats *stats)
{
	struct vmtotal  vminfo;
	struct swapent *swapdev;
	static int       mib[] = { CTL_VM, VM_METER };
	size_t           size;
	int              nswaps, total_mem;

	/* update mem usage */
	size = sizeof(vminfo);
	if (sysctl(mib, 2, &vminfo, &size, NULL, 0) < 0)
		err(1, "sysinfo update: VM.METER failed");

	stats->active  = vminfo.t_arm    << sys_pageshift;
	stats->free    = vminfo.t_free   << sys_pageshift;
	stats->total   = vminfo.t_rm     << sys_pageshift;
	total_mem = stats->active + stats->free + stats->total;

	/* update mem percents */
	stats->active_pct = (float)stats->active / (float)total_mem * 100.0;
	stats->free_pct   = (float)stats->free   / (float)total_mem * 100.0;
	stats->total_pct  = (float)stats->total  / (float)total_mem * 100.0;

	/* update swap usage */
	if (0 != (nswaps = swapctl(SWAP_NSWAP, 0, 0))) {
		if ((swapdev = calloc(nswaps, sizeof(*swapdev))) == NULL)
			err(1, "swapdev calloc failed (%d)", nswaps);

		if (swapctl(SWAP_STATS, swapdev, nswaps) == -1)
			err(1, "swapctl(SWAP_STATS)");

		int isize = 0;
		for (isize = 0; isize < nswaps; isize++) {
			if (swapdev[isize].se_flags & SWF_ENABLE) {
				stats->swap_used  = swapdev[isize].se_inuse
					          / (1024 / DEV_BSIZE);
				stats->swap_total = swapdev[isize].se_nblks
					          / (1024 / DEV_BSIZE);
			}
		}
		free(swapdev);
	}

	/* update swap percents and strings */
	stats->swap_used_pct = (float)stats->swap_used
	                     / (float)stats->swap_total
	                     * 100.0;
}

void
memory_close(__attribute__((unused)) struct memory_stats *stats)
{
	/* nothing to do here */
}
