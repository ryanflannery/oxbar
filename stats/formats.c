#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "formats.h"

char*
fmttimespan(int minutes)
{
   char *ret;

   if (-1 == minutes) {
      if (NULL == (ret = strdup("?")))
         err(1, "%s: strdup failed", __FUNCTION__);

      return ret;
   }

   int h = minutes / 60;
   int m = minutes % 60;

   if (-1 == asprintf(&ret, "%dh %dm", h, m))
      err(1, "%s: asprintf failed", __FUNCTION__);

   return ret;
}

char*
fmtpercent(float p)
{
   char *ret;

   if (-1 == asprintf(&ret, "%3.0f%%", p))
      err(1, "%s: asprintf failed", __FUNCTION__);

   return ret;
}


char*
fmtmemory(int kbytes)
{
   static const char *suffixes[] = { "K", "M", "G", "T" };
   size_t snum    = sizeof(suffixes) / sizeof(suffixes[0]);
   double dbytes  = (double) kbytes;
   size_t step    = 0;

   if (1024 < kbytes) {
      for (step = 0; (kbytes / 1024) > 0 && step < snum; step++, kbytes /= 1024)
         dbytes = kbytes / 1024.0;
   }

   char *ret;
   if (-1 == asprintf(&ret, "%6.1lf%s", dbytes, suffixes[step]))
      err(1, "%s: asprintf failed for %d", __FUNCTION__, kbytes);

   return ret;
}

char*
fmtint(int i)
{
   char *ret;

   if (-1 == asprintf(&ret, "%d", i))
      err(1, "%s:: asprintf failed", __FUNCTION__);

   return ret;
}

