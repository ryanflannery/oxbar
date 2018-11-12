#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include "brightness.h"

int
main()
{
   brightness_init();
   if (!BRIGHTNESS.is_setup)
      errx(1, "failed to setup brightness!");

   printf("bright%%\n");

   while (1)
   {
      brightness_update();
      printf("%5.1f\n", BRIGHTNESS.brightness);
      sleep(1);
   }

   brightness_close();
}
