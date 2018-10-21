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
   xdisp_t *x = xdisp_init();
   xfont_t *f;

   printf("display: %x x %x (pixels)\n", x->display_width, x->display_height);

   f = xfont_init("serif italic 20");

   xwin_t *w = xwin_init(x, "foobar", 500, 500, 500, 500);
   xctx_t *root = xctx_init(f, w, L2R, 10, true);

   xctx_root_push(root);

   xdraw_color(root, "#ffffff");
   xdraw_printf(root, "#ff0000", "Hello world!");

   xctx_root_pop(root);
   xcb_flush(x->con);

   pause();

   xctx_root_push(root);

   xdraw_color(root, "#ffffff");
   xdraw_printf(root, "#00ff00", "Hello world!");

   xctx_root_pop(root);
   xcb_flush(x->con);

   pause();

   printf("bye\n");
   xctx_free(root);
   xwin_free(x, w);
   xdisp_free(x);
   return 0;
}
