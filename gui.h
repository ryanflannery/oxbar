#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include "settings.h"
#include "gui/xcore.h"
#include "stats/stats.h"

typedef struct oxbarui {
   /* not packed */
   xinfo_t *xinfo;

   /* core settings (static after init) */
   settings_t *settings;

   /* state */
   double   xcurrent;

   /* TODO Create settings abstraction
    * Once all settings abstracted away (to settings.* ?) remove the below.
    * These are hangovers from my migration to abstracting all settings into
    * a new component, settings.h|c, which the gui would then use. My approach
    * though is...tedious. I've only migrated core X setup (x,y,width,height)
    * and the battery widget to use the new settings.* component. As such,
    * the other widgets, and the below settings, still need seperate handling.
    */
   int widget_padding;
   int small_space;
   char *fgcolor;
   char *bgcolor;
} oxbarui_t;

oxbarui_t* ui_create(settings_t *s);
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

void
ui_widget_net_draw(
      oxbarui_t  *ui,
      net_info_t *net);

void
ui_widget_time_draw(
      oxbarui_t  *ui);

#endif
