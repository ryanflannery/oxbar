#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"

oxbarui_t*
ui_create(
      const char *wname,
      int         x,
      int         y,
      int         width,
      int         height,
      int         padding,
      double      fontpt,
      const char *font,
      const char *bgcolor,
      const char *fgcolor
      )
{
   oxbarui_t *ui = malloc(sizeof(oxbarui_t));
   if (NULL == ui)
      err(1, "%s: couldn't malloc ui", __FUNCTION__);

   ui->xinfo = malloc(sizeof(xinfo_t));
   if (NULL == ui->xinfo)
      err(1, "%s: couldn't malloc xinfo", __FUNCTION__);

   ui->bgcolor = strdup(bgcolor);
   ui->fgcolor = strdup(fgcolor);
   if (NULL == ui->bgcolor || NULL == ui->fgcolor)
      err(1, "%s: strdup failed", __FUNCTION__);

   /* XXX These need to be done in a specific order */
   xcore_setup_x_connection_screen_visual(ui->xinfo);

   if (-1 == height)
      height = (uint32_t)(ceil(fontpt + (2 * padding)));

   if (-1 == y)
      y = ui->xinfo->display_height - height;

   if (-1 == width)
      width = ui->xinfo->display_width;

   ui->xinfo->padding = padding;

   xcore_setup_x_window(
         ui->xinfo,
         wname,
         x, y,
         width, height);
   xcore_setup_x_wm_hints(ui->xinfo);
   xcore_setup_cairo(ui->xinfo);
   xcore_setup_xfont(ui->xinfo, font, fontpt);

   /* now map the window & do an initial paint */
   xcb_map_window(ui->xinfo->xcon, ui->xinfo->xwindow);

   return ui;
}

void
ui_destroy(oxbarui_t *ui)
{
   free(ui->bgcolor);
   free(ui->fgcolor);
   xcore_destroy(ui->xinfo);
   free(ui->xinfo);
   free(ui);
}
