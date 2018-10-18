#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "xdraw.h"

static void
add_widget(widget_list_t *list, widget_t *w)
{
   if (list->size >= MAX_WIDGETS)
      errx(1, "%s: too many widgets", __FUNCTION__);

   list->widgets[ list->size++ ] = w;
}

gui_t*
gui_init(
      xinfo_t *xinfo,
      xfont_t *xfont,
      xwin_t  *xwin,
      char    *bgcolor,
      char    *widget_bgcolor,
      unsigned int margin,
      unsigned int padding)
{
   gui_t *gui = malloc(sizeof(gui_t));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->xinfo = xinfo;
   gui->xfont = xfont;
   gui->xwin  = xwin;

   gui->root = xctx_init(xfont, xwin, L2R, padding, true);

   gui->bgcolor = bgcolor;
   gui->widget_bgcolor = widget_bgcolor;

   gui->margins.top     = margin;
   gui->margins.bottom  = margin;
   gui->margins.left    = margin;
   gui->margins.right   = margin;

   gui->paddings.top    = padding;
   gui->paddings.bottom = padding;
   gui->paddings.left   = padding;
   gui->paddings.right  = padding;

   gui->LeftWidgets.size = 0;
   gui->RightWidgets.size = 0;
   gui->CenterWidgets.size = 0;

   return gui;
}

void
gui_free(gui_t *gui)
{
   free(gui);
}

void
gui_add_widget(gui_t *gui, xctx_align_t align, widget_t* w)
{
   switch (align) {
   case LEFT:     add_widget(&gui->LeftWidgets, w);   break;
   case RIGHT:    add_widget(&gui->RightWidgets, w);  break;
   case CENTER:   add_widget(&gui->CenterWidgets, w); break;
   }
}

static void
draw_widget(gui_t *gui, xctx_t *dest, widget_t *w)
{
   if (!w->enabled(w))
      return;

   xctx_t *scratchpad = xctx_init(gui->xfont, gui->xwin, L2R, gui->paddings.top, false);
   xdraw_color(scratchpad, gui->widget_bgcolor);
   w->draw(w, scratchpad);
   xdraw_hline(scratchpad, w->hdcolor, scratchpad->paddings.top, 0, scratchpad->xoffset);
   xdraw_outline(scratchpad, 0, 0, scratchpad->xoffset, scratchpad->h);
   xdraw_context(dest, scratchpad);
   xctx_free(scratchpad);
}

static void
draw_widget_list(
      gui_t         *gui,
      xctx_direction_t direction,
      widget_list_t *list)
{
   xctx_t *root = xctx_init(gui->xfont, gui->xwin, direction, gui->paddings.top, true);
   xctx_t *temp = xctx_init(gui->xfont, gui->xwin, L2R, gui->paddings.top, false);

   size_t i = 0;
   for (; i < list->size; i++) {
      draw_widget(gui, temp, list->widgets[i]);
      if (i != list->size - 1)
         xctx_advance(temp, AFTER_RENDER, gui->margins.left + gui->margins.right, 0);
   }

   xdraw_outline(temp, 0, 0, temp->xoffset, temp->h);
   xdraw_context(root, temp);
   xctx_free(temp);
   xctx_free(root);
}

void
gui_draw(gui_t *gui)
{
   xctx_root_push(gui->root);
   xdraw_color(gui->root, gui->bgcolor);
   draw_widget_list(gui, L2R,       &gui->LeftWidgets);
   draw_widget_list(gui, R2L,       &gui->RightWidgets);
   draw_widget_list(gui, CENTERED,  &gui->CenterWidgets);
   xctx_root_pop(gui->root);
   xcb_flush(gui->xinfo->con);
}
