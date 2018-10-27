#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include "nprocs.h"

int
main()
{
   nprocs_init();
   if (!NPROCS.is_setup)
      errx(1, "failed to setup core!");

   printf("%8s\n", "procs");

   while (1)
   {
      nprocs_update();
      printf("%8d\n", NPROCS.nprocs);
      sleep(1);
   }

   nprocs_close();
}
