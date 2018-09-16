#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "tseries.h"

tseries_t*
tseries_init(size_t size)
{
   tseries_t  *t;
   size_t      i;

   if (0 == size)
      errx(1, "%s: invalid parameter %zu", __FUNCTION__, size);

   if (NULL == (t = malloc(sizeof(tseries_t))))
      err(1, "%s: malloc failed", __FUNCTION__);

   t->size = size;
   if (NULL == (t->values = calloc(size, sizeof(double))))
      err(1, "%s: calloc failed", __FUNCTION__);

   for (i = 0; i < size; i++) t->values[i] = 0;

   return t;
}

void
tseries_update(tseries_t *t, double v)
{
   t->current = (t->current + 1) % t->size;
   t->values[t->current] = v;
}

void
tseries_free(tseries_t *t)
{
   free(t->values);
   free(t);
}

void
tseries_print(tseries_t *t)
{
   size_t i;
   for (i = 0; i < t->size; i++)
      printf("%3.1f ", t->values[i]);

   printf("\n");
}
