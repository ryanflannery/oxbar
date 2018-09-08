#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include "nprocs.h"
#include "formats.h"

nprocs_info_t NPROCS;

void
nprocs_init()
{
   NPROCS.nprocs = 0;
   NPROCS.str_nprocs = NULL;

   NPROCS.is_setup = true;
}

void
nprocs_update()
{
   static int mib[] = { CTL_KERN, KERN_NPROCS };

   /* update number of processes */
   size_t size = sizeof(NPROCS.nprocs);
   if (sysctl(mib, 2, &NPROCS.nprocs, &size, NULL, 0) == -1)
      warn("nprocs_update: sysctl KERN.NPROCS");

   if (NULL != NPROCS.str_nprocs) free(NPROCS.str_nprocs);
   NPROCS.str_nprocs = fmtint(NPROCS.nprocs);
}

void
nprocs_close()
{
   /* TODO free shiz */
}
