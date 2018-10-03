#ifndef GUI_H
#define GUI_H

#include <sys/queue.h>

#include "widgets.h"
#include "settings.h"
#include "gui/xcore.h"
#include "gui/xdraw.h"
#include "stats/stats.h"

typedef struct widget_list {
#define MAX_WIDGETS 100
   widget_t *widgets[MAX_WIDGETS];
   size_t    size;
} widget_list_t;

typedef struct gui {
   xinfo_t     *xinfo;     /* WHERE to draw     */
   settings_t  *settings;  /* HOW to draw       */

   /* WHAT to draw, and in WHICH contexts */
   widget_list_t LeftWidgets;
   widget_list_t CenterWidgets;
   widget_list_t RightWidgets;
} gui_t;

gui_t* gui_init(settings_t *s);
void gui_free(gui_t *gui);
void gui_add_widget(gui_t *gui, widget_t *w, xctx_direction_t direction);
void gui_draw(gui_t *gui);

#endif
