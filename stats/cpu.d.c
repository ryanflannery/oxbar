#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include "cpu.h"

int
main()
{
   int i;
   cpu_init();
   if (!CPUS.is_setup)
      errx(1, "failed to setup cpus!");

   printf("# cpus = %d\n\n", CPUS.ncpu);

   printf("%4s: %7s %7s %7s %7s %7s",
         "cpuX", "user", "nice", "system", "intrpt", "idle\n");

   cpu_update();
   cpu_update();

   while (1)
   {
      cpu_update();
      for (i = 0; i < CPUS.ncpu; i++) {
         printf("cpu%1d: %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%%\n",
               i,
               CPUS.cpus[i].percentages[CP_USER],
               CPUS.cpus[i].percentages[CP_NICE],
               CPUS.cpus[i].percentages[CP_SYS],
               CPUS.cpus[i].percentages[CP_INTR],
               CPUS.cpus[i].percentages[CP_IDLE]);
      }
      printf("\n");

      sleep(1);
   }

   cpu_close();
}
