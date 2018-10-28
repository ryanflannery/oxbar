#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "xdraw.h"

static void
add_widget(struct widget_list *list, struct widget *w)
{
   if (list->size >= MAX_WIDGETS)
      errx(1, "%s: too many widgets", __FUNCTION__);

   list->widgets[ list->size++ ] = w;
}

struct gui*
gui_init(struct xfont *xfont, struct xwin *xwin, struct gui_settings *settings)
{
   struct gui *gui = malloc(sizeof(struct gui));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->xfont  = xfont;
   gui->xwin   = xwin;
   gui->s      = settings;

   gui->LeftWidgets.size = 0;
   gui->RightWidgets.size = 0;
   gui->CenterWidgets.size = 0;
   return gui;
}

void
gui_free(struct gui *gui)
{
   free(gui);
}

void
gui_add_widget(struct gui *gui, xctx_direction_t direction, struct widget* w)
{
   switch (direction) {
   case L2R:      add_widget(&gui->LeftWidgets, w);   break;
   case R2L:      add_widget(&gui->RightWidgets, w);  break;
   case CENTERED: add_widget(&gui->CenterWidgets, w); break;
   }
}

static void
draw_widget(struct gui *gui, struct xctx *dest, struct widget *w)
{
   if (!w->enabled(w))
      return;

   struct xctx *scratchpad = xctx_init_scratchpad(gui->xfont, gui->xwin, L2R, &gui->s->padding);
   xdraw_colorfill(scratchpad, gui->s->widget_bgcolor);
   w->draw(w, scratchpad);
   xctx_complete(scratchpad);
   xdraw_headerline(scratchpad, gui->s->header_style, w->hdcolor);
   xdraw_context(dest, scratchpad);
   xctx_free(scratchpad);
}

static void
draw_widget_list(
      struct gui          *gui,
      struct widget_list  *list,
      xctx_direction_t     direction)
{
   struct xctx *root = xctx_init_root(gui->xfont, gui->xwin, direction, &gui->s->margin);
   struct xctx *temp = xctx_init_scratchpad(gui->xfont, gui->xwin, L2R, NULL);

   size_t i = 0;
   for (; i < list->size; i++) {
      draw_widget(gui, temp, list->widgets[i]);
      if (i != list->size - 1)
         xctx_advance(temp, AFTER_RENDER, gui->s->widget_spacing, 0);
   }

   xctx_complete(temp);
   xdraw_context(root, temp);
   xctx_free(temp);
   xctx_free(root);
}

void
gui_draw(struct gui *gui)
{
   xwin_push(gui->xwin);
   draw_widget_list(gui, &gui->LeftWidgets,     L2R);
   draw_widget_list(gui, &gui->RightWidgets,    R2L);
   draw_widget_list(gui, &gui->CenterWidgets,   CENTERED);
   xwin_pop(gui->xwin);
}
