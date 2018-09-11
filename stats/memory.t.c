#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include "memory.h"

int
main()
{
   memory_init();
   if (!MEMORY.is_setup)
      errx(1, "failed to setup memory!");

   printf("%8s %7s ",   "Active",   "%");
   printf("%8s %7s ",   "Total",    "%");
   printf("%8s %7s ",   "Free",     "%");
   printf("%8s %7s ",   "Swap U",   "%");
   printf("%8s\n",      "Swap T");


   while (1)
   {
      memory_update();
      printf("%8d %7.1f ",
            MEMORY.active,
            MEMORY.active_pct);
      printf("%8d %7.1f ",
            MEMORY.total,
            MEMORY.total_pct);
      printf("%8d %7.1f ",
            MEMORY.free,
            MEMORY.free_pct);
      printf("%8d %7.1f ",
            MEMORY.swap_used,
            MEMORY.swap_used_pct);
      printf("%8d\n",
            MEMORY.swap_total);

      sleep(1);
   }

   memory_close();
}
