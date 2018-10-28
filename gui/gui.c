#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "xdraw.h"

struct gui*
gui_init(struct xfont *xfont, struct xwin *xwin, struct gui_settings *settings)
{
   struct gui *gui = malloc(sizeof(struct gui));
   if (NULL == gui)
      err(1, "%s: couldn't malloc gui", __FUNCTION__);

   gui->xfont  = xfont;
   gui->xwin   = xwin;
   gui->s      = settings;

   TAILQ_INIT(&gui->left);
   TAILQ_INIT(&gui->center);
   TAILQ_INIT(&gui->right);
   return gui;
}

static void
free_widget_list(struct widget_list *widgets)
{
   struct widget_list_entry *wle = NULL;
   while (!TAILQ_EMPTY(widgets)) {
      wle = TAILQ_FIRST(widgets);
      TAILQ_REMOVE(widgets, wle, widget_entry);
      free(wle);
   }
}

void
gui_free(struct gui *gui)
{
   free_widget_list(&gui->left);
   free_widget_list(&gui->center);
   free_widget_list(&gui->right);
   free(gui);
}

void
gui_add_widget(struct gui *gui, xctx_direction_t direction, struct widget* w)
{
   struct  widget_list_entry *wle;
   if (NULL == (wle = malloc(sizeof(struct widget_list_entry))))
      err(1, "failed to allocate widget list entry");

   wle->widget = w;
   switch (direction) {
   case L2R:      TAILQ_INSERT_TAIL(&gui->left,   wle, widget_entry); break;
   case CENTERED: TAILQ_INSERT_TAIL(&gui->center, wle, widget_entry); break;
   case R2L:      TAILQ_INSERT_TAIL(&gui->right,  wle, widget_entry); break;
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
      struct gui           *gui,
      struct widget_list   *widgets,
      xctx_direction_t      direction)
{
   struct widget_list_entry *wle = NULL;
   struct xctx *root = xctx_init_root(gui->xfont, gui->xwin, direction, &gui->s->margin);
   struct xctx *temp = xctx_init_scratchpad(gui->xfont, gui->xwin, L2R, NULL);

   TAILQ_FOREACH(wle, widgets, widget_entry) {
      draw_widget(gui, temp, wle->widget);
      if (NULL != TAILQ_NEXT(wle, widget_entry))
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
   draw_widget_list(gui, &gui->left,   L2R);
   draw_widget_list(gui, &gui->center, CENTERED);
   draw_widget_list(gui, &gui->right,  R2L);
   xwin_pop(gui->xwin);
}
