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
gui_init(char *wmname, char *bgcolor, char *font,
      int x, int y, int w, int h, int padding,
      int widget_spacing,
      char *widget_bgcolor)
{
   gui_t *gui = malloc(sizeof(gui_t));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->xinfo = xcore_init(
         wmname,
         x, y,
         w, h,
         padding,
         bgcolor,
         font);

   gui->widget_spacing = widget_spacing;
   gui->widget_bgcolor = widget_bgcolor;
   gui->LeftWidgets.size = 0;
   gui->RightWidgets.size = 0;
   gui->CenterWidgets.size = 0;

   return gui;
}

void
gui_free(gui_t *gui)
{
   xcore_free(gui->xinfo);
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

   xctx_t *scratchpad = xctx_init(gui->xinfo, L2R, gui->widget_bgcolor, false);
   w->draw(w, scratchpad);
   xdraw_hline(scratchpad, w->hdcolor, scratchpad->padding, 0, scratchpad->xoffset);
   xdraw_context(dest, scratchpad);
   xctx_free(scratchpad);
}

static void
draw_widget_list(
      gui_t            *gui,
      xctx_direction_t  direction,
      widget_list_t    *list)
{
   xctx_t *root = xctx_init(gui->xinfo, direction, NULL, true);
   xctx_t *temp = xctx_init(gui->xinfo, L2R, NULL, false);

   size_t i = 0;
   for (; i < list->size; i++) {
      draw_widget(gui, temp, list->widgets[i]);
      if (i != list->size - 1)
         xctx_advance(temp, AFTER_RENDER, gui->widget_spacing, 0);
   }

   xdraw_context(root, temp);
   xctx_free(temp);
   xctx_free(root);
}

void
gui_draw(gui_t *gui)
{
   xcore_clear(gui->xinfo);
   draw_widget_list(gui, L2R,      &gui->LeftWidgets);
   draw_widget_list(gui, R2L,      &gui->RightWidgets);
   draw_widget_list(gui, CENTERED, &gui->CenterWidgets);
   xcore_flush(gui->xinfo);
}
