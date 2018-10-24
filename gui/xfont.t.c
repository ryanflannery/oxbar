#include <err.h>
#include <stdio.h>

#include "xcore.h"

void
test_font(const char *description)
{
   xfont_t *f = xfont_init(description);
   printf("%s :: family '%s' height: %d\n",
         0 == f->height ? "FAILED!" : "GOOD",
         pango_font_description_get_family(f->pfont),
         f->height);
   xfont_free(f);
}

int
main()
{
   /*
   test_font("");             // fails!
   test_font("fixed");        // fails! need height spec
   test_font("fixed 16pt");   // fails! 'pt' should be 'px' or empty
   test_font("fixed 16px");
   test_font("times 16px");
   test_font("DejaVu Sans Mono 16px");
   */

   size_t lsize = 100;
   char *line = malloc(lsize);
   if (NULL == line)
      err(1, "malloc failed");

   printf("Now enter font descriptions to see how pango parses them.\n");
   printf("Enter one per line and hit enter. We'll use pango to parse\n");
   printf("it and see if it loads.\n\n");
   printf("Hit control-d to exit\n");
   for (printf("$ "); -1 != getline(&line, &lsize, stdin); printf("$ "))
      test_font(line);

   printf("bye\n");
   free(line);
   return 0;
}
