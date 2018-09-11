#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "histogram.h"

histogram_t*
histogram_init(size_t nsamples, size_t nseries)
{
   histogram_t *h;
   size_t       i, j;

   if (0 == nseries || 0 == nsamples)
      errx(1, "%s: invalid parameter %zu %zu", __FUNCTION__, nseries, nsamples);

   if (NULL == (h = malloc(sizeof(histogram_t))))
      err(1, "%s: malloc failed", __FUNCTION__);

   h->nseries  = nseries;
   h->nsamples = nsamples;
   h->current  = 0;

   if (NULL == (h->series = calloc(nsamples, sizeof(double*))))
      err(1, "%s: calloc failed", __FUNCTION__);

   for (i = 0; i < nsamples; i++)
      if (NULL == (h->series[i] = calloc(nseries, sizeof(double))))
         err(1, "%s: calloc failed (2)", __FUNCTION__);

   for (i = 0; i < nsamples; i++)
      for (j = 0; j < nseries; j++)
         h->series[i][j] = 0.0;
      /*bzero(h->series[i], sizeof(double) * nseries);*/

   return h;
}

void
histogram_update(histogram_t *h, double data[])
{
   size_t cur = (h->current + 1) % h->nsamples;

   for (size_t i = 0; i < h->nseries; i++)
      h->series[cur][i] = data[i];

   h->current = cur;
}

void
histogram_free(histogram_t *h)
{
   size_t i;
   for (i = 0; i < h->nsamples; i++)
      free(h->series[i]);

   free(h->series);
   free(h);
}

void
histogram_print(histogram_t *h)
{
   size_t i, j;
   for (i = 0; i < h->nseries; i++) {
      for (j = 0; j < h->nsamples; j++)
         printf("%3.1f ", h->series[j][i]);

      printf("\n");
   }
}
