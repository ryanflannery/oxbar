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

   printf("%8s\t%8s\t%8s\t%8s%8s\n", "plugged?", "%", "\%str", "minutes", "str");

   while (1)
   {
      battery_update();
      printf("%8s\t%8.1f\t%8s\t%8d%8s\n",
            BATTERY.plugged_in ? "TRUE" : "false",
            BATTERY.charge_pct,
            BATTERY.str_charge_pct,
            BATTERY.minutes_remaining,
            BATTERY.str_time_remaining);

      sleep(1);
   }

   battery_close();
}
