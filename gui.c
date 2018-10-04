#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"

static void
add_widget(widget_list_t *list, widget_t *w)
{
   if (list->size >= MAX_WIDGETS)
      errx(1, "%s: too many widgets", __FUNCTION__);

   list->widgets[ list->size++ ] = w;
}

static void
free_widget_list(widget_list_t *list)
{
   size_t i = 0;
   for (; i < list->size; i++)
      list->widgets[i]->free(list->widgets[i]);
}

gui_t*
gui_init(settings_t *s)
{
   gui_t *gui = malloc(sizeof(gui_t));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->xinfo = xcore_init(
         s->display.wmname,
         s->display.x, s->display.y,
         s->display.w, s->display.h,
         s->display.padding_top,
         s->display.bgcolor,
         s->display.font);

   gui->LeftWidgets.size = 0;
   gui->RightWidgets.size = 0;
   gui->CenterWidgets.size = 0;

   return gui;
}

void
gui_free(gui_t *gui)
{
   free_widget_list(&gui->LeftWidgets);
   free_widget_list(&gui->RightWidgets);
   free_widget_list(&gui->CenterWidgets);
   xcore_free(gui->xinfo);
   free(gui);
}

void
gui_add_widget(gui_t *gui, xctx_direction_t direction, widget_t* w)
{
   switch (direction) {
   case L2R:
      add_widget(&gui->LeftWidgets, w);
      break;
   case R2L:
      add_widget(&gui->RightWidgets, w);
      break;
   case CENTERED:
      add_widget(&gui->CenterWidgets, w);
      break;
   }
}

static void
draw_widget(gui_t *gui, xctx_t *dest, widget_t *w)
{
   if (!w->enabled(w))
      return;

   xctx_t *scratchpad = xctx_init(gui->xinfo, L2R, false);
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
   xctx_t *root = xctx_init(gui->xinfo, direction, true);
   xctx_t *temp = xctx_init(gui->xinfo, L2R, false);

   size_t i = 0;
   for (; i < list->size; i++) {
      draw_widget(gui, temp, list->widgets[i]);
      if (i != list->size - 1)
         xctx_advance(temp, AFTER_RENDER,  15, 0); /* TODO => widget_spacing */
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
