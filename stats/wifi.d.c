#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include "wifi.h"

int
main()
{
   wifi_init();
   if (!WIFI.is_setup)
      errx(1, "failed to setup wifi!");

   printf("strength%%\n");

   while (1)
   {
      wifi_update();
      printf("%3u%%\n", WIFI.signal_strength);

      sleep(1);
   }

   wifi_close();
}
