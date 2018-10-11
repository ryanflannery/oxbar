#include <stdio.h>

#include "xcore.h"

void
test_font(const char *description)
{
   xfont_t *f = xfont_init(description);
   printf("'%s' => (%d)\n", f->ufont, f->height);
   xfont_free(f);
}

int
main()
{
   test_font("");             /* fails! height => 0 */
   test_font("fixed");        /* fails! height => 0 */
   test_font("fixed 16pt");   /* fails! height => 0 */
   test_font("fixed 16px");
   test_font("times 16px");
   test_font("DejaVu Sans Mono 16px");
   return 0;
}
