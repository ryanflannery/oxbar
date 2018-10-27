#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include "battery.h"

int
main()
{
   battery_init();
   if (!BATTERY.is_setup)
      errx(1, "failed to setup battery!");

   printf("%8s\t%8s\t%8s\n", "plugged?", "%", "minutes");

   while (1)
   {
      battery_update();
      printf("%8s\t%8.1f\t%8d\n",
            BATTERY.plugged_in ? "TRUE" : "false",
            BATTERY.charge_pct,
            BATTERY.minutes_remaining);

      sleep(1);
   }

   battery_close();
}
