
#include "xcore.h"

typedef struct oxbarui {
   /* not packed */
   xinfo_t  *xinfo;

   char     *bgcolor;
   char     *fgcolor;
} oxbarui_t;

oxbarui_t*
ui_create(
      const char *wname,   /* name for window in window manager            */
      int         x,       /* (x,y) top-left pixel coordinates for oxbar   */
      int         y,
      int         width,   /* (width,height) of oxbar in pixels            */
      int         height,
      int         padding, /* internal padding between text and border     */
      double      fontpt,  /* font-size in classic point scale             */
      const char *font,    /* font specified by user (natively)            */

      const char *bgcolor,
      const char *fgcolor
      );

void ui_destroy(oxbarui_t *ui);

