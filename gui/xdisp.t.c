#include <stdio.h>

#include "xcore.h"

int
main()
{
   xdisp_t *x = xdisp_init();
   printf("display: %x x %x (pixels)\n", x->display_width, x->display_height);
   xdisp_free(x);
   return 0;
}
