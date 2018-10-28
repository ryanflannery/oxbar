/*
 * Test driver (system test) for chart.* component
 * Run by just:
 *  ./chart.d
 */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "chart.h"

int
main()
{
   const char *colors[] = {
      "#ff0000",
      "#00ff00",
      "#0000ff"
   };

   struct chart *c;

   c = chart_init(
         10, 3,      /* 10 samples of 3 different series */
         false,      /* raw values (not percentages) */
         "#555555",
         colors);

   double i = 1;
   while (1) {
      chart_update(c, (double[]){ i, i + 1, i + 2 });
      i++;
      chart_print(c);
      sleep(1);
   }

   chart_free(c);
   return 0;
}
