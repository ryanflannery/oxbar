#ifndef WIDGETS_H
#define WIDGETS_H

#include "settings.h"
#include "stats/stats.h"
#include "gui/xdraw.h"

/* widget components */
typedef bool (*widget_enabled_t)(oxstats_t*);   /* does this widget work?     */
typedef void (*widget_init_t)(settings_t*);     /* do widget init on startup  */
typedef void (*widget_free_t)(void*);           /* cleanup widget on shutdown */
typedef void (*widget_draw_t)(xctx_t*, void*, settings_t*, oxstats_t*); /* draw it! */

/* the full widget type */
typedef struct widget {
   const char       *name;
   widget_enabled_t  enabled;
   widget_init_t     init;
   widget_free_t     free;
   widget_draw_t     draw;

   /* TODO i could make oxstats_t and settings_t members on init? cleaner? */
   char*             hdcolor;
   void*             data; /* per-widget use (_init/_free's responsibility) */
} widget_t;

/* global list of all *available* widgets / settings determine which are used */
extern widget_t WIDGETS[];       /* TODO should be const - fix settings! */
extern const size_t  NWIDGETS;

#endif
