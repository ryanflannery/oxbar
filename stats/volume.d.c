#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include "volume.h"

int
main()
{
   volume_init();
   if (!VOLUME.is_setup)
      errx(1, "failed to setup volume!");

   printf("%8s\t%8s\t%8s\n", "mute?", "left", "right");

   while (1)
   {
      volume_update();
      printf("%8s\t%8.1f\t%8.1f\n",
            VOLUME.muted ? "TRUE" : "false",
            VOLUME.left,
            VOLUME.right);

      sleep(1);
   }

   volume_close();
}
