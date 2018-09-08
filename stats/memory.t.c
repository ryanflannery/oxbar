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

   printf("%8s %7s %7s ",  "Active",   "str", "%");
   printf("%8s %7s %7s ",  "Total",    "str", "%");
   printf("%8s %7s %7s ",  "Free",     "str", "%");
   printf("%8s %7s %7s ",  "Swap U",   "str", "%");
   printf("%8s %7s\n",     "Swap T",   "str");


   while (1)
   {
      memory_update();
      printf("%8d %7s %7.1f ",
            MEMORY.active,
            MEMORY.str_active,
            MEMORY.active_pct);
      printf("%8d %7s %7.1f ",
            MEMORY.total,
            MEMORY.str_total,
            MEMORY.total_pct);
      printf("%8d %7s %7.1f ",
            MEMORY.free,
            MEMORY.str_free,
            MEMORY.free_pct);
      printf("%8d %7s %7.1f ",
            MEMORY.swap_used,
            MEMORY.str_swap_used,
            MEMORY.swap_used_pct);
      printf("%8d %7s\n",
            MEMORY.swap_total,
            MEMORY.str_swap_total);

      sleep(1);
   }

   memory_close();
}
