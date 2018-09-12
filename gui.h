#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include "gui/xcore.h"
#include "stats/stats.h"

typedef struct oxbarui {
   /* not packed */
   xinfo_t *xinfo;

   /* core settings (static after init) */
   char    *bgcolor;
   char    *fgcolor;
   double   widget_padding;
   double   small_space;

   /* state */
   double   xcurrent;
} oxbarui_t;

oxbarui_t*
ui_create(
      const char *wname,   /* name for window in window manager            */
      int         x,       /* (x,y) top-left pixel coordinates for oxbar   */
      int         y,
      int         width,   /* (width,height) of oxbar in pixels            */
      int         height,
      int         padding, /* internal padding between text and border     */
      const char *font,    /* font specified by user (natively)            */

      const char *bgcolor,
      const char *fgcolor
      );

void ui_destroy(oxbarui_t *ui);

void ui_clear(oxbarui_t *ui);
void ui_flush(oxbarui_t *ui);

void
ui_widget_battery_draw(
      oxbarui_t      *ui,
      battery_info_t *battery);

void
ui_widget_volume_draw(
      oxbarui_t      *ui,
      volume_info_t *volume);

void
ui_widget_nprocs_draw(
      oxbarui_t      *ui,
      nprocs_info_t  *nprocs);

void
ui_widget_memory_draw(
      oxbarui_t      *ui,
      memory_info_t  *memory);

void
ui_widget_cpus_draw(
      oxbarui_t  *ui,
      cpus_t     *cpus);

#endif
