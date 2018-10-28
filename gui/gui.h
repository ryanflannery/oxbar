#ifndef GUI_H
#define GUI_H

#include "xcore.h"
#include "xdraw.h"

/* the full widget type */
struct widget {
   const char       *name;             /* only used for debugging             */
   char             *hdcolor;          /* header color (set by settings.*)    */
   bool (*enabled)(struct widget*);    /* does the widget work? could change! */
   void (*draw)(struct widget*, struct xctx*); /* draw it to a context!       */
   struct widget_context *context;     /* local per-widget state              */
};

struct widget_list {
#define MAX_WIDGETS 100
   struct widget *widgets[MAX_WIDGETS];
   size_t         size;
};

/* pad that wraps all user-customizable settings for a gui */
struct gui_settings {
   char          *widget_bgcolor;
   int            widget_spacing;
   struct padding padding;
   struct padding margin;
   header_style_t header_style;
};

/* a gui just sets-up X stuff and orchestrates widgets */
struct gui {
   struct gui_settings *s;
   struct xfont        *xfont;
   struct xwin         *xwin;

   /* WHAT to draw, and in WHICH contexts */
   struct widget_list LeftWidgets;
   struct widget_list CenterWidgets;
   struct widget_list RightWidgets;
};

struct gui* gui_init(struct xfont *xfont, struct xwin *xwin, struct gui_settings *settings);
void gui_free(struct gui *gui);
void gui_add_widget(struct gui *gui, xctx_direction_t direction, struct widget *w);
void gui_draw(struct gui *gui);

#endif
