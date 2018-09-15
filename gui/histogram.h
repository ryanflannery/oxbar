#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <sys/types.h>

/*
 * A histogram is an N x M array of percentags (doubles), where
 * N = number of series
 * M = number of samples of each series
 *
 * Each sample is a percentage, represented as a double.
 * The sum of all series for a given sample should sum to 100.
 */
typedef struct histogram {
   size_t   nseries;    /* how many series the histogram has         */
   size_t   nsamples;   /* how many samples of each series there are */
   size_t   current;    /* index of current column in series         */
   double **series;     /* the nseries x nsamples array of percents  */
} histogram_t;

histogram_t* histogram_init(size_t nsamples, size_t nseries);
void histogram_update(histogram_t *h, double data[]);
void histogram_free(histogram_t *h);
void histogram_print(histogram_t *h);

#endif
