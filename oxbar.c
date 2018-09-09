#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "ui.h"
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
histogram_update(histogram_t *h, double data[])
{
   size_t cur = (h->current + 1) % h->nsamples;

   for (size_t i = 0; i < h->nseries; i++)
      h->series[cur][i] = data[i];

   h->current = cur;
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
   for (i = 0; i < h->nseries; i++) {
      for (j = 0; j < h->nsamples; j++)
         printf("%3.1f ", h->series[j][i]);

      printf("\n");
   }
}

int
histogram_draw(oxbarui_t *ui, histogram_t *h, double x)
{
   double r, g, b, a;
   int width = h->nsamples;

   /* paint green to start */
   hex2rgba("859900", &r, &g, &b, &a);
   cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, a);
   cairo_rectangle(ui->xinfo->cairo,
         x,
         ui->xinfo->padding,
         width,
         ui->xinfo->fontpt);
   cairo_fill(ui->xinfo->cairo);

   return width;
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
   /* inputs */
   const char *bgcolor  = "1c1c1c"; /*"#212429";*/
   const char *fgcolor  = "93a1a1"; /*"839496"; "#cc5500";*/
   const char *font     = "DejaVu Sans";
   double font_size     = 16.0;
   double bar_padding   = 5.0;
   int bar_x            = 100;
   int bar_y            = 100;   /* -1 means "bottom"          */
   int bar_width        = 1500;  /* -1 means "display width"   */
   int bar_height       = -1;    /* determined by font size (is that right?) */

   oxbarui_t *ui;
   ui = ui_create(
         getprogname(),
         bar_x, bar_y,
         bar_width, bar_height,
         bar_padding,
         font_size,
         font
         );

   stats_init();

   double x = bar_padding;
   double y = font_size + bar_padding;

   double p = 10.0;
   double s = 5.0;

   double sbarx;

   histogram_t *hist_memory = histogram_init(30, 3);

   while (1) {
      x = bar_padding;
      stats_update();

      double r, g, b, a;
      hex2rgba(bgcolor, &r, &g, &b, &a);
      cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, a);  /* FIXME */
      xcore_clear_background(ui->xinfo); /* FIXME */
      hex2rgba(fgcolor, &r, &g, &b, &a);
      cairo_set_source_rgba(ui->xinfo->cairo, r, g, b, a); /* FIXME */

      if (BATTERY.is_setup) {
         sbarx = x;
         x += ui_draw_text(ui,
               BATTERY.plugged_in ? fgcolor : "dc322f",
               x, y,
               BATTERY.plugged_in ? "AC:" : "BAT:");
         x += s;
         x += ui_draw_vertical_stack_bar(ui, x, BATTERY.charge_pct);
         x += s;
         x += ui_draw_text(ui, fgcolor, x, y, BATTERY.str_charge_pct);
         if (-1 != BATTERY.minutes_remaining) {
            x += s;
            x += ui_draw_text(ui, fgcolor, x, y, BATTERY.str_time_remaining);
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

         ui_draw_top_header(ui, "b58900", sbarx, x);
         x += p;
      }

      if (VOLUME.is_setup) {
         sbarx = x;
         x += ui_draw_text(ui, fgcolor, x, y, "VOLUME:");
         x += s;
         if (VOLUME.left_pct == VOLUME.right_pct) {
            x += ui_draw_vertical_stack_bar(ui, x, VOLUME.left_pct);
            x += s;
            x += ui_draw_text(ui, fgcolor, x, y, VOLUME.str_left_pct);
         } else {
            x += ui_draw_text(ui, fgcolor, x, y, VOLUME.str_left_pct);
            x += s;
            x += ui_draw_text(ui, fgcolor, x, y, VOLUME.str_right_pct);
         }
         ui_draw_top_header(ui, "cb4b16", sbarx, x);
         x += p;
      }

      if (NPROCS.is_setup) {
         sbarx = x;
         x += ui_draw_text(ui, fgcolor, x, y, "#PROCS:");
         x += s;
         x += ui_draw_text(ui, fgcolor, x, y, NPROCS.str_nprocs);
         ui_draw_top_header(ui, "dc322f", sbarx, x);
         x += p;
      }

      if (MEMORY.is_setup) {
         histogram_update(hist_memory, (double[]) {
               MEMORY.active_pct,
               MEMORY.total_pct,
               MEMORY.free_pct
               });
         histogram_print(hist_memory);
         sbarx = x;
         x += ui_draw_text(ui, fgcolor, x, y, "MEMORY:");
         x += s;
         /*
         x += histogram_draw(ui, hist_memory, x);
         x += s;
         */
         x += ui_draw_text(ui, "dc322f", x, y, MEMORY.str_active);
         x += s;
         x += ui_draw_text(ui, fgcolor, x, y, "active");
         x += s;
         x += ui_draw_text(ui, "b58900", x, y, MEMORY.str_total);
         x += s;
         x += ui_draw_text(ui, fgcolor, x, y, "total");
         x += s;
         x += ui_draw_text(ui, "859900", x, y, MEMORY.str_free);
         x += s;
         x += ui_draw_text(ui, fgcolor, x, y, "free");
         ui_draw_top_header(ui, "d33682", sbarx, x);
         x += p;
      }

      if (CPUS.is_setup) {
         sbarx = x;
         x += ui_draw_text(ui, fgcolor, x, y, "CPUS:");
         x += s;
         for (int i = 0; i < CPUS.ncpu; i++) {
            x += ui_draw_text(ui, fgcolor, x, y,
                  CPUS.cpus[i].str_percentages[CP_IDLE]);
            x += s;
         }
         ui_draw_top_header(ui, "6c71c4", sbarx, x);
         x += p;
      }

      cairo_surface_flush(ui->xinfo->csurface);
      xcb_flush(ui->xinfo->xcon);

      sleep(1);
   }

   histogram_free(hist_memory);
   stats_close();
   xcore_destroy_x(ui->xinfo);

   return 0;
}
