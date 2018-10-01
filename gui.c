#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui/xdraw.h"
#include "widgets.h"

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

   return gui;
}

void
gui_free(gui_t *gui)
{
   xcore_free(gui->xinfo);
   free(gui);
}


static void
widgets_draw(
      gui_t            *gui,
      xctx_direction_t  direction,
      oxstats_t        *stats,
      settings_t       *settings,
      const widget_t  **widgets,
      size_t            nwidgets)
{
   xctx_t *root= xctx_init(gui->xinfo, direction, true);
   xctx_t *temp= xctx_init(gui->xinfo, L2R, false);
   size_t i = 0;
   for (i = 0; i < nwidgets; i++) {
      if (!widgets[i]->enabled(stats))
         continue;

      xctx_t *widget_context = xctx_init(gui->xinfo, L2R, false);
      widgets[i]->draw(widget_context, WIDGETS[i].data, settings, stats);
      xdraw_hline(widget_context, widgets[i]->hdcolor, widget_context->padding, 0, widget_context->xoffset);
      xctx_advance(widget_context, BEFORE_RENDER, 15, 0); /* TODO => widget_spacing */
      xctx_advance(widget_context, AFTER_RENDER,  15, 0); /* TODO => widget_spacing */
      xdraw_context(temp, widget_context);

      xctx_free(widget_context);
   }
   xdraw_context(root, temp);
   xctx_free(temp);
   xctx_free(root);
}

void
gui_draw(gui_t *gui)
{
   /* TODO gui macro-structure is hard-coded still. working on this */
   const widget_t *LeftWidgets[] = {
      &WIDGETS[2], /* nprocs */
      &WIDGETS[4], /* cpus */
   };
   const size_t nLeftWidgets = sizeof(LeftWidgets) / sizeof(widget_t*);

   const widget_t *CenteredWidgets[] = {
      &WIDGETS[1], /* volume */
      &WIDGETS[6], /* time */
      &WIDGETS[0], /* battery */
   };
   const size_t nCenteredWidgets = sizeof(CenteredWidgets) / sizeof(widget_t*);

   const widget_t *RightWidgets[] = {
      &WIDGETS[5], /* net */
      &WIDGETS[3], /* memory */
   };
   const size_t nRightWidgets = sizeof(RightWidgets) / sizeof(widget_t*);

   xcore_clear(gui->xinfo);

   widgets_draw(gui, L2R,      &OXSTATS, gui->settings, LeftWidgets,     nLeftWidgets);
   widgets_draw(gui, CENTERED, &OXSTATS, gui->settings, CenteredWidgets, nCenteredWidgets);
   widgets_draw(gui, R2L,      &OXSTATS, gui->settings, RightWidgets,    nRightWidgets);

   xcore_flush(gui->xinfo);
}
