#ifndef GUI_H
#define GUI_H

#include <sys/queue.h>

#include "settings.h"
#include "gui/xcore.h"
#include "gui/xdraw.h"
#include "stats/stats.h"

/* widget methods */
typedef void (*widget_init_t)(settings_t*);     /* do widget init on startup  */

/* the full widget type */
typedef struct widget {
   const char       *name;
   bool (*enabled)(struct widget*);       /* does this widget work?           */
   widget_init_t     init;
   void (*free)(struct widget*);          /* cleanup widget on shutdown       */
   void (*draw)(struct widget*, xctx_t*); /* draw it to a context!            */
   settings_t       *settings;
   oxstats_t        *stats;

   /* TODO i could make oxstats_t and settings_t members on init? cleaner? */
   char*             hdcolor;
   void*             data; /* per-widget use (_init/_free's responsibility) */
} widget_t;
typedef struct widget_list {

#define MAX_WIDGETS 100
   widget_t *widgets[MAX_WIDGETS];
   size_t    size;
} widget_list_t;

/* a gui just sets-up X stuff and orchestrates widgets */
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
void gui_add_widget(gui_t *gui, xctx_direction_t direction, widget_t *w);
void gui_draw(gui_t *gui);

#endif
