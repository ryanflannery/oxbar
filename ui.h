#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "xcore.h"
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
      double      fontpt,  /* font-size in classic point scale             */
      const char *font,    /* font specified by user (natively)            */

      const char *bgcolor,
      const char *fgcolor
      );

void ui_destroy(oxbarui_t *ui);

void ui_clear(oxbarui_t *ui);
void ui_flush(oxbarui_t *ui);

void
ui_widget_battery_draw(
      oxbarui_t  *ui,
      bool        plugged_in,
      double      charge_pct,
      const char *str_charge_pct,
      int         minutes_remaining,
      const char *str_time_remaining);

void
ui_widget_volume_draw(
      oxbarui_t  *ui,
      double      left_pct,
      double      right_pct,
      const char *str_left_pct,
      const char *str_right_pct);

void
ui_widget_nprocs_draw(
      oxbarui_t  *ui,
      const char *str_nprocs);

void
ui_widget_memory_draw(
      oxbarui_t  *ui,
      double      free_pct,
      double      total_pct,
      double      active_pct,
      const char *str_free,
      const char *str_total,
      const char *str_active);

void
ui_widget_cpus_draw(
      oxbarui_t  *ui,
      cpus_t     *cpus);

#endif
