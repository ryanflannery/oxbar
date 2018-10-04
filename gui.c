#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"

void
add_widget(widget_list_t *list, widget_t *w)
{
   if (list->size >= MAX_WIDGETS)
      errx(1, "%s: too many widgets", __FUNCTION__);

   list->widgets[ list->size++ ] = w;
}

gui_t*
gui_init(settings_t *s)
{
   gui_t *gui = malloc(sizeof(gui_t));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->settings = s;
   gui->xinfo = xcore_init(
         gui->settings->display.wmname,
         gui->settings->display.x, gui->settings->display.y,
         gui->settings->display.w, gui->settings->display.h,
         gui->settings->display.padding_top,
         gui->settings->display.bgcolor,
         gui->settings->display.font);

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
draw_widget(gui_t *gui, xctx_t *dest, widget_t *w, oxstats_t *stats)
{
   if (!w->enabled(stats))
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
      oxstats_t        *stats,
      widget_list_t    *list)
{
   xctx_t *root = xctx_init(gui->xinfo, direction, true);
   xctx_t *temp = xctx_init(gui->xinfo, L2R, false);

   size_t i = 0;
   for (; i < list->size; i++) {
      draw_widget(gui, temp, list->widgets[i], stats);
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
   draw_widget_list(gui, L2R,      &OXSTATS, &gui->LeftWidgets);
   draw_widget_list(gui, R2L,      &OXSTATS, &gui->RightWidgets);
   draw_widget_list(gui, CENTERED, &OXSTATS, &gui->CenterWidgets);
   xcore_flush(gui->xinfo);
}
