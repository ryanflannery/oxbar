#include <stdio.h>
#include <unistd.h>

#include "xcore.h"

int
main()
{
   xinfo_t x;
   xinfo_open(&x);
   xwin_t *w = xwin_init(&x, "foobar", 500, 500, 500, 500);

   printf("display: %d x %d (in pixels)\n", x.display_width, x.display_height);

   /* draw background (light gray / alpha is 0.1) */
   cairo_set_source_rgba(w->cairo, 0, 0, 0, 0.1);
   cairo_paint(w->cairo);
   xcb_flush(x.con);

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

   cairo_paint(w->cairo);
   xcb_flush(x.con);

   pause();

   xwin_free(w);
   xinfo_close(&x);
   return 0;
}
/*
   cairo_set_operator(ctx->cairo, CAIRO_OPERATOR_SOURCE);
   cairo_paint(ctx->cairo);
   cairo_set_operator(ctx->cairo, CAIRO_OPERATOR_OVER);
*/
