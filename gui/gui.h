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

typedef struct widget_list {
#define MAX_WIDGETS 100
   widget_t *widgets[MAX_WIDGETS];
   size_t    size;
} widget_list_t;

/* if and how to show headers (the colored lines above widgets) */
typedef enum {
   NONE,    /* don't show them at all */
   TOP,     /* show the headers in the top widget padding */
   BOTTOM   /* show the headers in the bottom widget padding */
} header_style_t;

/* a gui just sets-up X stuff and orchestrates widgets */
typedef struct gui {
   xfont_t       *xfont;
   xwin_t        *xwin;
   char          *bgcolor;
   char          *widget_bgcolor;
   int            widget_spacing;
   padding_t      padding;
   padding_t      margin;
   header_style_t header_style;

   /* WHAT to draw, and in WHICH contexts */
   widget_list_t LeftWidgets;
   widget_list_t CenterWidgets;
   widget_list_t RightWidgets;
} gui_t;

gui_t*
gui_init(
      xfont_t       *xfont,
      xwin_t        *xwin,
      char          *bgcolor,
      char          *widget_bgcolor,
      int            widget_spacing,
      padding_t      padding,
      padding_t      margin,
      header_style_t header_style);

void gui_free(gui_t *gui);
void gui_add_widget(gui_t *gui, xctx_direction_t direction, widget_t *w);
void gui_draw(gui_t *gui);

#endif
