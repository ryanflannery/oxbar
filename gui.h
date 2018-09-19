#ifndef GUI_H
#define GUI_H

#include "settings.h"
#include "gui/xcore.h"
#include "gui/xdraw.h"
#include "stats/stats.h"

/*
 * Plan - i'm slowly moving more of this logic to xdraw/xcore components.
 * Goal is to make oxbarui/gui just a container of widgets that knows how to
 * call their render methods and stores their order, setting-up and passing
 * xcontext & xinfo to them and knowing their order. That's it.
 */
typedef struct gui {
   /* not packed */
   xinfo_t          *xinfo;
   xdraw_context_t  *xcontext;

   /* core settings (static after init) */
   settings_t *settings;

   /* state */
   int      space_width;   /* width of a space " " in the font */
} gui_t;

gui_t* gui_init(settings_t *s);
void gui_free(gui_t *gui);

/* the main draw method - renders the whole display */
void gui_draw(gui_t *gui);

#endif
