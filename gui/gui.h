#ifndef GUI_H
#define GUI_H

#include <sys/queue.h>

#include "xcore.h"
#include "xdraw.h"

/*
 * A widget, as understood by the gui. It has:
 *    name:    A string name, used solely for debugging.
 *    hdcolor: A color used optionally to draw a bar above/below the widget.
 *    enabled: A method to determine if the widget is enabled or not. Widgets
 *             can appear/disappear based on stats/state.
 *    draw:    A method to render the widget onto a x context.
 */
struct widget {
   const char       *name;             /* only used for debugging             */
   char            **hdcolor;          /* header color (set by settings.*)    */
   char            **bgcolor;          /* backround color (set by settings.*) */
   char            **fgcolor;          /* backround color (set by settings.*) */
   bool (*enabled)(void *);            /* does the widget work? can change!   */
   void (*draw)(void *, struct xctx*); /* draw it to a context!               */
   void             *state;            /* internal state to the widget        */
};

/* pad that wraps all user-customizable settings for a gui */
struct gui_settings {
   char          *widget_bgcolor;
   int            widget_spacing;
   struct padding padding;
   struct padding margin;
   header_style_t header_style;
};

struct widget_list_entry {
   TAILQ_ENTRY(widget_list_entry) widget_entry;
   struct widget *widget;
};

/* a gui just contains & orchestrates the drawing of widgets */
struct gui {
   struct gui_settings *s;
   struct xfont        *xfont;
   struct xwin         *xwin;

   /* list of widgets in the left, center, and right lists */
   TAILQ_HEAD(widget_list, widget_list_entry) left, center, right;
};

struct gui* gui_init(struct xfont *, struct xwin *, struct gui_settings *);
void gui_free(struct gui *);
void gui_add_widget(struct gui *, xctx_direction_t, struct widget *);
void gui_draw(struct gui *);

#endif
