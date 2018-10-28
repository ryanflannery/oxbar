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

   xfont_settings_t fonts = {
      .desc = "serif italic 20",
      .fgcolor = "ff0000"
   };
   f = xfont_init(&fonts);

   xwin_settings_t wins = {
      .bgcolor = "#ff0000",
      .wname = "xcore.d :: xwin test",
      .x = 500, .y = 500,
      .w = 500, .h = 500
   };
   padding_t padding = {
      .top = 10,
      .right = 10,
      .bottom = 10,
      .left = 10
   };
   xwin_t *w = xwin_init(x, &wins);
   xctx_t *root = xctx_init_root(f, w, L2R, &padding);

   xwin_push(w);

   xdraw_colorfill(root, "#ffffff");
   xdraw_printf(root, "#ff0000", "Hello world!");

   xwin_pop(w);

   pause();

   xwin_push(w);

   xdraw_colorfill(root, "#ffffff");
   xdraw_printf(root, "#00ff00", "Hello world!");

   xwin_pop(w);

   pause();

   printf("bye\n");
   xctx_free(root);
   xwin_free(w);
   xdisp_free(x);
   return 0;
}
