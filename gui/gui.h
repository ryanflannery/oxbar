#ifndef GUI_H
#define GUI_H

#include "xcore.h"
#include "xdraw.h"

/* the full widget type */
typedef struct widget {
   const char       *name;             /* only used for debugging             */
   char             *hdcolor;          /* header color (set by settings.*)    */
   bool (*enabled)(struct widget*);    /* does the widget work? could change! */
   void (*draw)(struct widget*, xctx_t*); /* draw it to a context!            */
   struct widget_context *context;     /* local per-widget state              */
} widget_t;

/* container for a list of widgets */
typedef struct widget_list {
#define MAX_WIDGETS 100
   widget_t *widgets[MAX_WIDGETS];
   size_t    size;
} widget_list_t;

/* definition for padding & margins (common for all widgets) */
typedef padding_t margin_t;

/* a gui just sets-up X stuff and orchestrates widgets */
typedef struct gui {
   xinfo_t  *xinfo;
   xfont_t  *xfont;
   xwin_t   *xwin;
   xctx_t   *root;
   char     *widget_bgcolor;
   margin_t  margins;
   padding_t paddings;
   char     *bgcolor;

   /* WHAT to draw, and in WHICH contexts */
   widget_list_t LeftWidgets;
   widget_list_t CenterWidgets;
   widget_list_t RightWidgets;
} gui_t;

gui_t*
gui_init(
      xinfo_t *xinfo,
      xfont_t *xfont,
      xwin_t  *xwin,
      char    *bgcolor,
      char    *widget_bgcolor,
      unsigned int margin,
      unsigned int padding);

void gui_free(gui_t *gui);
void gui_add_widget(gui_t *gui, xctx_align_t align, widget_t *w);
void gui_draw(gui_t *gui);

#endif
