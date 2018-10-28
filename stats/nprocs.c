#include <err.h>
#include <stdio.h>
#include <sys/sysctl.h>

#include "nprocs.h"

struct nprocs_info NPROCS;

void
nprocs_init()
{
   NPROCS.nprocs = 0;
   NPROCS.is_setup = true;
}

void
nprocs_update()
{
   static int mib[] = { CTL_KERN, KERN_NPROCS };
   static size_t size = sizeof(NPROCS.nprocs);

   /* update number of processes */
   if (sysctl(mib, 2, &NPROCS.nprocs, &size, NULL, 0) == -1)
      warn("%s: sysctl KERN.NPROCS", __FUNCTION__);
}

void
nprocs_close()
{
   /* nothing to do */
}
