#include <stdio.h>

#include "xdraw.h"

void
pause()
{
   printf("press ENTER to continue");
   getchar();
}

int
main()
{
   xinfo_t x;
   xfont_t *f;

   xinfo_open(&x);
   printf("display: %d x %d (in pixels)\n", x.display_width, x.display_height);

   f = xfont_init("serif italic 20");

   xwin_t *w = xwin_init(&x, "foobar", 500, 500, 500, 500);
   xctx_t *root = xctx_init(&x, f, w, L2R, 10, true);

   xctx_root_push(root);

   xdraw_color(root, "#ffffff");
   xdraw_printf(root, "#ff0000", "Hello world!");

   xctx_root_pop(root);
   xcb_flush(x.con);

   pause();

   xctx_root_push(root);

   xdraw_color(root, "#ffffff");
   xdraw_printf(root, "#00ff00", "Hello world!");

   xctx_root_pop(root);
   xcb_flush(x.con);

   pause();

   printf("bye\n");
   xctx_free(root);
   xwin_free(w);
   xinfo_close(&x);
   return 0;
}
