/*
 * Test driver (system test) for xcore.* components.
 * Compile and run:
 * ./xcore.d [ xfont | xdisp | xwin ]
 *
 * xfont
 *    Let's you test typing in font descriptions and see if they can load
 *    and what they load to. If any font fails, process exits.
 *
 * xdisp
 *    Connect to display and reports width/height of full X display.
 *
 * xwin
 *    Sample showing connection to xcb, making a window, and then testing
 *    the cairo setup within xwin.
 */

#include <err.h>
#include <stdio.h>

#include "xcore.h"

void
test_font(char *description)
{
   struct xfont_settings s = {
      .desc = description,
      .fgcolor = "ff0000"
   };
   struct xfont *f = xfont_init(&s);
   printf("%s :: family '%s' height: %d\n",
         0 == f->height ? "FAILED!" : "GOOD",
         pango_font_description_get_family(f->pfont),
         f->height);
   xfont_free(f);
}

int
main_xfont()
{
   size_t lsize = 100;
   char *line = malloc(lsize);
   if (NULL == line)
      err(1, "malloc failed");

   printf("Now enter font descriptions to see how pango parses them.\n");
   printf("Enter one per line and hit enter. We'll use pango to parse\n");
   printf("it and see if it loads.\n\n");
   printf("Hit control-d to exit\n");
   for (printf("$ "); -1 != getline(&line, &lsize, stdin); printf("$ "))
      test_font(line);

   printf("bye\n");
   free(line);
   return 0;
}


int
main_xdisp()
{
   struct xdisp *x = xdisp_init();
   printf("display: %d x %d (pixels)\n", x->display_width, x->display_height);
   xdisp_free(x);
   return 0;
}

int
main_xwin()
{
   struct xdisp *x = xdisp_init();
   printf("display: %d x %d (pixels)\n", x->display_width, x->display_height);

   struct xwin_settings s = {
      .bgcolor = "#ff0000",
      .wname = "xcore.d :: xwin test",
      .x = 500, .y = 500,
      .w = 500, .h = 500
   };
   struct xwin *w = xwin_init(x, &s);

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
   xwin_free(w);
   xdisp_free(x);
   return 0;
}

int
main(int argc, char *argv[])
{
   if (2 != argc) {
      printf("usage: %s [ xfont | xdisp | xwin ]\n", argv[0]);
      return 1;
   }

   if (0 == strcmp("xfont", argv[1]))
      return main_xfont();
   else if (0 == strcmp("xdisp", argv[1]))
      return main_xdisp();
   else if (0 == strcmp("xwin", argv[1]))
      return main_xwin();

   printf("unknown option '%s'\n", argv[1]);
   return 1;
}
