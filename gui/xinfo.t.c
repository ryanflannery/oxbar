#include <stdio.h>

#include "xcore.h"

int
main()
{
   xinfo_t x;
   xinfo_open(&x);
   printf("display: %x x %x (in pixels)\n", x.display_width, x.display_height);
   xinfo_close(&x);
   return 0;
}
