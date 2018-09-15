#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/swap.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/vmmeter.h>

#include "memory.h"

memory_info_t MEMORY;
int   sys_pageshift;

void
memory_init()
{
   int pgsize;

   /* setup page-shift */
   pgsize = getpagesize();
   sys_pageshift = 0;
   while (pgsize > 1) {
      sys_pageshift++;
      pgsize >>= 1;
   }
   sys_pageshift -= 10;

   /* default values: raw */
   MEMORY.active = MEMORY.free = MEMORY.total = 0;
   MEMORY.swap_used = MEMORY.swap_total = 0;

   /* default values: percentages */
   MEMORY.active_pct = MEMORY.free_pct = 0.0;
   MEMORY.swap_used_pct = 0.0;

   MEMORY.is_setup = true;
}

void
memory_update()
{
   struct vmtotal vminfo;
   struct swapent *swapdev;
   static int mib[] = { CTL_VM, VM_METER };
   size_t size;
   int    nswaps, isize, total_mem;

   /* update mem usage */
   size = sizeof(vminfo);
   if (sysctl(mib, 2, &vminfo, &size, NULL, 0) < 0)
      err(1, "sysinfo update: VM.METER failed");

   MEMORY.active  = vminfo.t_arm    << sys_pageshift;
   MEMORY.free    = vminfo.t_free   << sys_pageshift;
   MEMORY.total   = vminfo.t_rm     << sys_pageshift;
   total_mem = MEMORY.active + MEMORY.free + MEMORY.total;

   /* update mem percents */
   MEMORY.active_pct = (float)MEMORY.active / (float)total_mem * 100.0;
   MEMORY.free_pct   = (float)MEMORY.free   / (float)total_mem * 100.0;
   MEMORY.total_pct  = (float)MEMORY.total  / (float)total_mem * 100.0;

   /* update swap usage */
   if (0 != (nswaps = swapctl(SWAP_NSWAP, 0, 0))) {
      if ((swapdev = calloc(nswaps, sizeof(*swapdev))) == NULL)
         err(1, "sysinfo update: swapdev calloc failed (%d)", nswaps);
      if (swapctl(SWAP_STATS, swapdev, nswaps) == -1)
         err(1, "sysinfo update: swapctl(SWAP_STATS) failed");

      for (isize = 0; isize < nswaps; isize++) {
         if (swapdev[isize].se_flags & SWF_ENABLE) {
            MEMORY.swap_used  = swapdev[isize].se_inuse / (1024 / DEV_BSIZE);
            MEMORY.swap_total = swapdev[isize].se_nblks / (1024 / DEV_BSIZE);
        }
      }
      free(swapdev);
   }

   /* update swap percents and strings */
   MEMORY.swap_used_pct = (float)MEMORY.swap_used / (float)MEMORY.swap_total * 100.0;
}

void
memory_close()
{
   /* TODO MEMORY cleanup routine should free/cleanup shiz */
}
