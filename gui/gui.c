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
      xfont_t *xfont,
      xwin_t *xwin,
      char *bgcolor,
      char *widget_bgcolor,
      int   widget_spacing,
      padding_t padding,
      padding_t margin,
      header_style_t header_style)
{
   gui_t *gui = malloc(sizeof(gui_t));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->xfont = xfont;
   gui->xwin  = xwin;

   gui->margin = margin;
   gui->padding = padding;
   gui->bgcolor = bgcolor;
   gui->widget_bgcolor = widget_bgcolor;
   gui->widget_spacing = widget_spacing;
   gui->header_style = header_style;
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
gui_add_widget(gui_t *gui, xctx_direction_t direction, widget_t* w)
{
   switch (direction) {
   case L2R:      add_widget(&gui->LeftWidgets, w);   break;
   case R2L:      add_widget(&gui->RightWidgets, w);  break;
   case CENTERED: add_widget(&gui->CenterWidgets, w); break;
   }
}

static void
draw_widget(gui_t *gui, xctx_t *dest, widget_t *w)
{
   if (!w->enabled(w))
      return;

   xctx_t *scratchpad = xctx_init(gui->xfont, gui->xwin, L2R,
         gui->xfont->height + (gui->padding.top + gui->padding.bottom), gui->padding, false);
   xdraw_color(scratchpad, gui->widget_bgcolor);
   w->draw(w, scratchpad);
   xctx_complete(scratchpad);

   if (ABOVE == gui->header_style)
      xdraw_hline(scratchpad, w->hdcolor, scratchpad->padding.top, 0, 0, scratchpad->xoffset);
   else if (BELOW == gui->header_style)
      xdraw_hline(scratchpad, w->hdcolor, scratchpad->padding.bottom, scratchpad->h, 0, scratchpad->xoffset);

   xdraw_context(dest, scratchpad);
   xctx_free(scratchpad);
}

static void
draw_widget_list(
      gui_t            *gui,
      widget_list_t    *list,
      xctx_direction_t  direction)
{
   static const padding_t ZEROPAD = { .top = 0, .bottom = 0, .left = 0, .right = 0 };
   xctx_t *root = xctx_init(gui->xfont, gui->xwin, direction, gui->xwin->h, gui->margin, true);
   xctx_t *temp = xctx_init(gui->xfont, gui->xwin, L2R,
         gui->xfont->height + (gui->padding.top + gui->padding.bottom), ZEROPAD, false);

   size_t i = 0;
   for (; i < list->size; i++) {
      draw_widget(gui, temp, list->widgets[i]);
      if (i != list->size - 1)
         xctx_advance(temp, AFTER_RENDER, gui->widget_spacing, 0);
   }

   xctx_complete(temp);
   xdraw_context(root, temp);
   xctx_free(temp);
   xctx_free(root);
}

void
gui_draw(gui_t *gui)
{
   xwin_push(gui->xwin);
   draw_widget_list(gui, &gui->LeftWidgets,     L2R);
   draw_widget_list(gui, &gui->RightWidgets,    R2L);
   draw_widget_list(gui, &gui->CenterWidgets,   CENTERED);
   xwin_pop(gui->xwin);
}
