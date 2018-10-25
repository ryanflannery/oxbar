#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chart.h"

chart_t*
chart_init(
      size_t   nsamples,
      size_t   nseries,
      bool     is_percents,
      const char  *bgcolor,
      const char **colors)
{
   chart_t *c;
   size_t   i, j;

   if (0 == nseries || 0 == nsamples || NULL == bgcolor || NULL == colors)
      errx(1, "%s: invalid parameter %zu %zu", __FUNCTION__, nseries, nsamples);

   if (NULL == (c = malloc(sizeof(chart_t))))
      err(1, "%s: malloc failed", __FUNCTION__);

   c->nseries  = nseries;
   c->nsamples = nsamples;
   c->percents = is_percents;
   c->current  = nsamples - 1;    /* first update will be at 0 */

   if (NULL == (c->values = calloc(nsamples, sizeof(double*))))
      err(1, "%s: calloc failed", __FUNCTION__);

   for (i = 0; i < nsamples; i++)
      if (NULL == (c->values[i] = calloc(nseries, sizeof(double))))
         err(1, "%s: calloc failed (2)", __FUNCTION__);

   for (i = 0; i < nsamples; i++)
      for (j = 0; j < nseries; j++)
         c->values[i][j] = 0.0;

   if (NULL == (c->bgcolor = strdup(bgcolor)))
      err(1, "%s: strdup failed", __FUNCTION__);

   if (NULL == (c->colors = calloc(nsamples, sizeof(char*))))
      err(1, "%s: colors calloc failed", __FUNCTION__);

   for (j = 0; j < nseries; j++) {
      if (NULL == (c->colors[j] = strdup(colors[j])))
      err(1, "%s: colors strdup failed(2)", __FUNCTION__);
   }

   return c;
}

void
chart_free(chart_t *c)
{
   size_t i;
   for (i = 0; i < c->nsamples; i++)
      free(c->values[i]);

   free(c->values);
   free(c->bgcolor);
   free(c);
}

void
chart_update(chart_t *c, double data[])
{
   size_t cur = (c->current + 1) % c->nsamples;
   size_t i;

   for (i = 0; i < c->nseries; i++) {
      if (0 > data[i])
         errx(1, "%s: charts don't support negative values\n", __FUNCTION__);

      c->values[cur][i] = data[i];
   }

   c->current = cur;
}

void
chart_get_minmax(chart_t *c, double *min, double *max)
{
   size_t i, j;
   *min = *max = c->values[0][0];
   for (i = 0; i < c->nsamples; i++) {
      for (j = 0; j < c->nseries; j++) {
         if (c->values[i][j] > *max)
            *max = c->values[i][j];

         if (c->values[i][j] < *min)
            *min = c->values[i][j];
      }
   }
}

void
chart_print(chart_t *c)
{
   size_t i, j;

   printf("nseries: %zu\t\tnsamples: %zu\n", c->nseries, c->nsamples);
   printf("current: %zu\t\tpercents? %s\n", c->current, c->percents ? "YES" : "NO");
   printf("bgcolor: '%s'\n", c->bgcolor);
   for (i = 0; i < c->nseries; i++)
      printf("  color[%zu] = '%s'\n", i, c->colors[i]);

   for (i = 0; i < c->nseries; i++) {
      for (j = 0; j < c->nsamples; j++)
         printf("%3.1f ", c->values[j][i]);

      printf("\n");
   }
}
