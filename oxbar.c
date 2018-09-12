#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "gui.h"
#include "stats/stats.h"

int
main ()
{
   /* inputs :: these will be made configurable */
   const char *bgcolor  = "1c1c1c"; /*"#212429";*/
   const char *fgcolor  = "93a1a1"; /*"839496"; "#cc5500";*/
   const char *font     = "DejaVu Sans 18px";
   double bar_padding   = 10;
   int bar_x            = 0;
   int bar_y            = -1; /* -1 means "bottom"          */
   int bar_width        = -1; /* -1 means "display width"   */
   int bar_height       = 28; /* TODO Nice if i could support determining based
                                 on font size...but i haven't figured that
                                 out yet w/ pango
                              */

   oxbarui_t *ui = ui_create(
         getprogname(),
         bar_x, bar_y,
         bar_width, bar_height,
         bar_padding,
         font,
         bgcolor, fgcolor
         );

   stats_init();

   while (1) {
      stats_update();

      ui_clear(ui);

      if (BATTERY.is_setup)
         ui_widget_battery_draw(ui, &BATTERY);

      if (VOLUME.is_setup)
         ui_widget_volume_draw(ui, &VOLUME);

      if (NPROCS.is_setup)
         ui_widget_nprocs_draw(ui, &NPROCS);

      if (MEMORY.is_setup)
         ui_widget_memory_draw(ui, &MEMORY);

      if (CPUS.is_setup)
         ui_widget_cpus_draw(ui, &CPUS);

      ui_flush(ui);
      sleep(1);
   }

   stats_close();
   ui_destroy(ui);

   return 0;
}

/*
 * TODO keeping for posterity - my attempt at rendering an image
 * see example in notes - i got it after this
double scale = 0.65;
cairo_surface_t *png = cairo_image_surface_create_from_png("battery_charging.png");
cairo_scale(xinfo.cairo, scale, scale);
cairo_set_source_surface(xinfo.cairo, png, x, 0);
x += cairo_image_surface_get_width(png);
cairo_scale(xinfo.cairo, 1.0/scale, 1.0/scale);
cairo_paint(xinfo.cairo);
*/
