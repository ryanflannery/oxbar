#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "xdraw.h"
#include "histogram.h"
#include "stats/stats.h"

int
main ()
{
   /* inputs :: these will be made configurable */
   const char *bgcolor  = "1c1c1c"; /*"#212429";*/
   const char *fgcolor  = "93a1a1"; /*"839496"; "#cc5500";*/
   const char *font     = "DejaVu Sans";
   double font_size     = 16.0;
   double bar_padding   = 5.0;
   int bar_x            = 100;
   int bar_y            = 100;   /* -1 means "bottom"          */
   int bar_width        = 1500;  /* -1 means "display width"   */
   int bar_height       = -1;    /* determined by font size (is that right?) */

   static const char *single_bar_colors[] = {   /* volume + battery */
      "dc322f",
      "859900"
   };

   static const char *memory_bar_colors[] = {
      "859900",
      "bbbb00",
      "dc322f"
   };

   static const char *cpu_bar_colors[] = {
      "859900",   /* idle */
      "d33682",   /* interrupt */
      "b58900",   /* sys  */
      "2aa198",   /* nice */
      "dc322f"    /* user */
   };

   oxbarui_t *ui = ui_create(
         getprogname(),
         bar_x, bar_y,
         bar_width, bar_height,
         bar_padding,
         font_size,
         font,
         bgcolor, fgcolor
         );

   stats_init();

   double x = bar_padding;
   double y = font_size + bar_padding;

   double p = 10.0;
   double s = 5.0;

   double sbarx;

   /* initialize histograms
    * i'm thinking more and more these should be moved to a "widget" library...
    * the idea being xcore.* remains what it does, ui.* becomes drawing
    * primitives (drawing text/lines/bars/etc) and then a "widgets.*" module
    * gets created to handle rendering and possibly state (like a histogram).
    * Still chewing.
    */
   histogram_t *hist_memory = histogram_init(60, 3);
   histogram_t *hist_cpu[CPUS.ncpu];
   for (int i = 0; i < CPUS.ncpu; i++)
      hist_cpu[i] = histogram_init(60, CPUSTATES);


   while (1) {
      x = bar_padding;
      stats_update();

      xdraw_clear_all(ui->xinfo, ui->bgcolor);

      if (BATTERY.is_setup) {
         sbarx = x;
         x += xdraw_text(ui->xinfo,
               BATTERY.plugged_in ? fgcolor : "dc322f",
               x, y,
               BATTERY.plugged_in ? "AC:" : "BAT:");
         x += s;
         x += xdraw_vertical_stack(ui->xinfo, x, 7, 2,
               single_bar_colors,
               (double[]){100.0 - BATTERY.charge_pct, BATTERY.charge_pct});
         x += s;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, BATTERY.str_charge_pct);
         if (-1 != BATTERY.minutes_remaining) {
            x += s;
            x += xdraw_text(ui->xinfo, fgcolor, x, y, BATTERY.str_time_remaining);
         }

         /* TODO keeping for posterity - my attempt at rendering an image
         double scale = 0.65;
         cairo_surface_t *png = cairo_image_surface_create_from_png("battery_charging.png");
         cairo_scale(xinfo.cairo, scale, scale);
         cairo_set_source_surface(xinfo.cairo, png, x, 0);
         x += cairo_image_surface_get_width(png);
         cairo_scale(xinfo.cairo, 1.0/scale, 1.0/scale);
         cairo_paint(xinfo.cairo);
         */

         xdraw_hline(ui->xinfo, "b58900b2", ui->xinfo->padding, sbarx, x);
         x += p;
      }

      if (VOLUME.is_setup) {
         sbarx = x;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, "VOLUME:");
         x += s;
         if (VOLUME.left_pct == VOLUME.right_pct) {
            x += xdraw_vertical_stack(ui->xinfo, x, 7, 2,
                  single_bar_colors,
                  (double[]){100.0 - VOLUME.left_pct, VOLUME.left_pct});
            x += s;
            x += xdraw_text(ui->xinfo, fgcolor, x, y, VOLUME.str_left_pct);
         } else {
            x += xdraw_text(ui->xinfo, fgcolor, x, y, VOLUME.str_left_pct);
            x += s;
            x += xdraw_text(ui->xinfo, fgcolor, x, y, VOLUME.str_right_pct);
         }
         xdraw_hline(ui->xinfo, "cb4b16b2", ui->xinfo->padding, sbarx, x);
         x += p;
      }

      if (NPROCS.is_setup) {
         sbarx = x;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, "#PROCS:");
         x += s;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, NPROCS.str_nprocs);
         xdraw_hline(ui->xinfo, "dc322fb2", ui->xinfo->padding, sbarx, x);
         x += p;
      }

      if (MEMORY.is_setup) {
         histogram_update(hist_memory, (double[]) {
               MEMORY.free_pct,
               MEMORY.total_pct,
               MEMORY.active_pct
               });
         sbarx = x;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, "MEMORY:");
         x += s;
         x += xdraw_histogram(ui->xinfo, x, memory_bar_colors, hist_memory);
         x += s;
         x += xdraw_text(ui->xinfo, "dc322f", x, y, MEMORY.str_active);
         x += s;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, "active");
         x += s;
         x += xdraw_text(ui->xinfo, "b58900", x, y, MEMORY.str_total);
         x += s;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, "total");
         x += s;
         x += xdraw_text(ui->xinfo, "859900", x, y, MEMORY.str_free);
         x += s;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, "free");
         xdraw_hline(ui->xinfo, "d33682b2", ui->xinfo->padding, sbarx, x);
         x += p;
      }

      if (CPUS.is_setup) {
         sbarx = x;
         x += xdraw_text(ui->xinfo, fgcolor, x, y, "CPUS:");
         x += s;
         for (int i = 0; i < CPUS.ncpu; i++) {
            histogram_update(hist_cpu[i], (double[]) {
                  CPUS.cpus[i].percentages[CP_IDLE],
                  CPUS.cpus[i].percentages[CP_INTR],
                  CPUS.cpus[i].percentages[CP_SYS],
                  CPUS.cpus[i].percentages[CP_NICE],
                  CPUS.cpus[i].percentages[CP_USER]
                  });
            x += xdraw_histogram(ui->xinfo, x, cpu_bar_colors, hist_cpu[i]);
            x += s;
            x += xdraw_text(ui->xinfo, fgcolor, x, y,
                  CPUS.cpus[i].str_percentages[CP_IDLE]);
            x += s;
         }
         xdraw_hline(ui->xinfo, "6c71c4b2", ui->xinfo->padding, sbarx, x);
         x += p;
      }

      xdraw_flush(ui->xinfo);
      sleep(1);
   }

   histogram_free(hist_memory);
   stats_close();
   ui_destroy(ui);

   return 0;
}
