#include <stdio.h>

#include "xcore.h"

int
main()
{
   xdisp_t *x = xdisp_init();
   printf("display: %x x %x (pixels)\n", x->display_width, x->display_height);

   xwin_t *w = xwin_init(x, "foobar", 500, 500, 500, 500);

   /* start double buffer - NOTE you must do this w/ xcb backend + alpha */
   cairo_push_group(w->cairo);

   /* draw background (light gray / alpha is 0.1) */
   cairo_set_source_rgba(w->cairo, 0, 0, 0, 0.1);
   cairo_paint(w->cairo);

   /* draw red square */
   cairo_set_source_rgba(w->cairo, 1, 0, 0, 0.5);
   cairo_rectangle(w->cairo, 50, 50, 200, 200);
   cairo_fill(w->cairo);

   /* draw green square */
   cairo_set_source_rgba(w->cairo, 0, 1, 0, 0.5);
   cairo_rectangle(w->cairo, 150, 150, 200, 200);
   cairo_fill(w->cairo);

   /* draw blue square */
   cairo_set_source_rgba(w->cairo, 0, 0, 1, 0.5);
   cairo_rectangle(w->cairo, 250, 250, 200, 200);
   cairo_fill(w->cairo);

   /* copy buffer back to root (that's all of the next 4 lines) */
   cairo_pop_group_to_source(w->cairo);
   cairo_set_operator(w->cairo, CAIRO_OPERATOR_SOURCE);
   cairo_paint(w->cairo);
   cairo_set_operator(w->cairo, CAIRO_OPERATOR_OVER);

   xcb_flush(x->con);

   printf("You should now see a transparent gray box with 3 squares.\n");
   printf("Move the windows behind the window to ensure the transparency is\n");
   printf("true and working correctly.\n\n");
   printf("Hit enter to continue");
   getchar();

   printf("bye\n");
   xwin_free(x, w);
   xdisp_free(x);
   return 0;
}
