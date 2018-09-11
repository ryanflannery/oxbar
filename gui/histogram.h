#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <sys/types.h>

typedef struct histogram {
   size_t   nseries;    /* how many series the histogram has         */
   size_t   nsamples;   /* how many samples of each series there are */
   size_t   current;    /* index of current column in series         */
   double **series;     /* the nseries x nsamples array of data      */
} histogram_t;

histogram_t* histogram_init(size_t nsamples, size_t nseries);
void histogram_update(histogram_t *h, double data[]);
void histogram_free(histogram_t *h);
void histogram_print(histogram_t *h);

#endif
