#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"
#include "widgets.h"

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
   gui_add_widget(gui, &WIDGETS[2], L2R); /* nprocs */
   gui_add_widget(gui, &WIDGETS[4], L2R); /* cpus */

   gui->RightWidgets.size = 0;
   gui_add_widget(gui, &WIDGETS[5], R2L); /* net */
   gui_add_widget(gui, &WIDGETS[3], R2L); /* memory */

   gui->CenterWidgets.size = 0;
   gui_add_widget(gui, &WIDGETS[1], CENTERED); /* volume */
   gui_add_widget(gui, &WIDGETS[6], CENTERED); /* time */
   gui_add_widget(gui, &WIDGETS[0], CENTERED); /* battery */

   return gui;
}

void
gui_free(gui_t *gui)
{
   xcore_free(gui->xinfo);
   free(gui);
}

void
gui_add_widget(gui_t *gui, widget_t *w, xctx_direction_t direction)
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
draw_widget(gui_t *gui, xctx_t *dest, const widget_t *w, oxstats_t *stats, settings_t *settings)
{
   if (!w->enabled(stats))
      return;

   xctx_t *widget_context = xctx_init(gui->xinfo, L2R, false);
   w->draw(widget_context, w->data, settings, stats);
   xdraw_hline(widget_context, w->hdcolor, widget_context->padding, 0, widget_context->xoffset);
   xctx_advance(widget_context, BEFORE_RENDER, 15, 0); /* TODO => widget_spacing */
   xctx_advance(widget_context, AFTER_RENDER,  15, 0); /* TODO => widget_spacing */
   xdraw_context(dest, widget_context);
   xctx_free(widget_context);
}

static void
draw_widget_list(
      gui_t            *gui,
      xctx_direction_t  direction,
      oxstats_t        *stats,
      settings_t       *settings,
      widget_list_t    *list)
{
   xctx_t *root = xctx_init(gui->xinfo, direction, true);
   xctx_t *temp = xctx_init(gui->xinfo, L2R, false);

   size_t i = 0;
   for (; i < list->size; i++)
      draw_widget(gui, temp, list->widgets[i], stats, settings);

   xdraw_context(root, temp);
   xctx_free(temp);
   xctx_free(root);
}

void
gui_draw(gui_t *gui)
{
   xcore_clear(gui->xinfo);

   draw_widget_list(gui, L2R,      &OXSTATS, gui->settings, &gui->LeftWidgets);
   draw_widget_list(gui, R2L,      &OXSTATS, gui->settings, &gui->RightWidgets);
   draw_widget_list(gui, CENTERED, &OXSTATS, gui->settings, &gui->CenterWidgets);

   xcore_flush(gui->xinfo);
}
