#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "xinfo.h"
#include "stats/stats.h"

typedef struct histogram {
   size_t   nseries;    /* how many series the histogram has         */
   size_t   nsamples;   /* how many samples of each series there are */
   size_t   current;    /* index of current column in series         */
   double **series;     /* the nseries x nsamples array of data      */
} histogram_t;

histogram_t*
histogram_init(size_t nsamples, size_t nseries)
{
   histogram_t *h;
   size_t       i, j;

   if (0 == nseries || 0 == nsamples)
      errx(1, "%s: invalid parameter %zu %zu", __FUNCTION__, nseries, nsamples);

   if (NULL == (h = malloc(sizeof(histogram_t))))
      err(1, "%s: malloc failed", __FUNCTION__);

   h->nseries  = nseries;
   h->nsamples = nsamples;
   h->current  = 0;

   if (NULL == (h->series = calloc(nsamples, sizeof(double*))))
      err(1, "%s: calloc failed", __FUNCTION__);

   for (i = 0; i < nsamples; i++)
      if (NULL == (h->series[i] = calloc(nseries, sizeof(double))))
         err(1, "%s: calloc failed (2)", __FUNCTION__);

   for (i = 0; i < nsamples; i++)
      for (j = 0; j < nseries; j++)
         h->series[i][j] = 0.0;
      /*bzero(h->series[i], sizeof(double) * nseries);*/

   return h;
}

void
histogram_free(histogram_t *h)
{
   size_t i;
   for (i = 0; i < h->nsamples; i++)
      free(h->series[i]);

   free(h->series);
   free(h);
}

void
histogram_print(histogram_t *h)
{
   size_t i, j;
   for (i = 0; i < h->nsamples; i++) {
      for (j = 0; j < h->nseries; j++)
         printf("%3.1f ", h->series[i][j]);

      printf("\n");
   }
}

void
histo_test()
{
   histogram_t *h = histogram_init(5, 10);
   histogram_print(h);
   histogram_free(h);
}

int
main ()
{
   histo_test();
   xinfo_t xinfo;

   /* inputs */
   const char *bgcolor  = "1c1c1c"; /*"#212429";*/
   const char *fgcolor  = "93a1a1"; /*"839496"; "#cc5500";*/
   const char *font     = "DejaVu Sans";
   double font_size     = 16.0;
   double bar_padding   = 5.0;
   int bar_x            = 100;
   int bar_y            = 100;   /* -1 means "bottom"          */
   int bar_width        = 1500;  /* -1 means "display width"   */
   int bar_height;               /* determined by font size (is that right?) */


   /* these need to be done in order */
   setup_x_connection_screen_visual(&xinfo);

   bar_height = (uint32_t)(ceil(font_size + (2 * bar_padding)));
   if (-1 == bar_y)     bar_y = xinfo.display_height - bar_height;
   if (-1 == bar_width) bar_width = xinfo.display_width;
   xinfo.bar_padding = bar_padding;

   setup_x_window(&xinfo, getprogname(), bar_x, bar_y, bar_width, bar_height);
   setup_x_wm_hints(&xinfo);
   setup_cairo(&xinfo);
   setup_xfont(&xinfo, font, font_size);

   /* now map the window & do an initial paint */
   xcb_map_window(xinfo.xcon, xinfo.xwindow);
   cairo_surface_flush(xinfo.csurface);
   xcb_flush(xinfo.xcon);

   stats_init();

   double x = bar_padding;
   double y = font_size + bar_padding;

   double p = 10.0;
   double s = 5.0;

   double sbarx;

   while (1) {
      x = bar_padding;
      stats_update();

      double r, g, b, a;
      hex2rgba(bgcolor, &r, &g, &b, &a);
      cairo_set_source_rgba(xinfo.cairo, r, g, b, a);
      clear_background(&xinfo);
      hex2rgba(fgcolor, &r, &g, &b, &a);
      cairo_set_source_rgba(xinfo.cairo, r, g, b, a);

      if (BATTERY.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo,
               BATTERY.plugged_in ? fgcolor : "dc322f",
               x, y,
               BATTERY.plugged_in ? "AC:" : "BAT:");
         x += s;
         x += window_draw_vertical_stack_bar(&xinfo, x, BATTERY.charge_pct);
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, BATTERY.str_charge_pct);
         if (-1 != BATTERY.minutes_remaining) {
            x += s;
            x += window_draw_text(&xinfo, fgcolor, x, y, BATTERY.str_time_remaining);
         }

         /*
         double scale = 0.65;
         cairo_surface_t *png = cairo_image_surface_create_from_png("battery_charging.png");
         cairo_scale(xinfo.cairo, scale, scale);
         cairo_set_source_surface(xinfo.cairo, png, x, 0);
         x += cairo_image_surface_get_width(png);
         cairo_scale(xinfo.cairo, 1.0/scale, 1.0/scale);
         cairo_paint(xinfo.cairo);
         */

         window_draw_top_header(&xinfo, "b58900", sbarx, x);
         x += p;
      }

      if (VOLUME.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "VOLUME:");
         x += s;
         if (VOLUME.left_pct == VOLUME.right_pct) {
            x += window_draw_vertical_stack_bar(&xinfo, x, VOLUME.left_pct);
            x += s;
            x += window_draw_text(&xinfo, fgcolor, x, y, VOLUME.str_left_pct);
         } else {
            x += window_draw_text(&xinfo, fgcolor, x, y, VOLUME.str_left_pct);
            x += s;
            x += window_draw_text(&xinfo, fgcolor, x, y, VOLUME.str_right_pct);
         }
         window_draw_top_header(&xinfo, "cb4b16", sbarx, x);
         x += p;
      }

      if (NPROCS.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "#PROCS:");
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, NPROCS.str_nprocs);
         window_draw_top_header(&xinfo, "dc322f", sbarx, x);
         x += p;
      }

      if (MEMORY.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "MEMORY:");
         x += s;
         x += window_draw_text(&xinfo, "dc322f", x, y, MEMORY.str_active);
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, "active");
         x += s;
         x += window_draw_text(&xinfo, "b58900", x, y, MEMORY.str_total);
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, "total");
         x += s;
         x += window_draw_text(&xinfo, "859900", x, y, MEMORY.str_free);
         x += s;
         x += window_draw_text(&xinfo, fgcolor, x, y, "free");
         window_draw_top_header(&xinfo, "d33682", sbarx, x);
         x += p;
      }

      if (CPUS.is_setup) {
         sbarx = x;
         x += window_draw_text(&xinfo, fgcolor, x, y, "CPUS:");
         x += s;
         for (int i = 0; i < CPUS.ncpu; i++) {
            x += window_draw_text(&xinfo, fgcolor, x, y,
                  CPUS.cpus[i].str_percentages[CP_IDLE]);
            x += s;
         }
         window_draw_top_header(&xinfo, "6c71c4", sbarx, x);
         x += p;
      }

      cairo_surface_flush(xinfo.csurface);
      xcb_flush(xinfo.xcon);

      sleep(1);
   }

   stats_close();
   destroy_x(&xinfo);

   return 0;
}
