#ifndef CHART_H
#define CHART_H

#include <stdbool.h>
#include <sys/types.h>

/*
 * A chart is a set of N sampls of M distinct series, where all sampled values
 * are doubles representing either percentages or raw values. If values are
 * percentages, they are on the scale of 0-100, not 0-1, and the values for
 * all series for a given sample should sum to 100 (or reasonably close).
 *
 * The chart also contains a background color (for the whole chart) as well
 * as a color for each series.
 *
 * Example:
 * If you sample the 3 memory stats (% active/total/free) for 60 seconds,
 * you would want a chart with nseries = 3, nsamples = 60, and percents = true.
 */
typedef struct chart {
   size_t   nseries;    /* how many distinct series are in the chart    */
   size_t   nsamples;   /* how many samples of each series there are    */
   size_t   current;    /* index of current SAMPLE                      */
   bool     percents;   /* true = all values are percents, false = raw  */
   double **values;     /* the nseries x nsamples array of percents     */
   char    *bgcolor;    /* background color of the chart                */
   char   **colors;     /* array of nsamples - colors for each sample   */
} chart_t;

chart_t* chart_init(
      size_t   nsamples,
      size_t   nseries,
      bool     is_percents,
      const char  *bgcolor,
      const char **colors);

void chart_free(chart_t *c);
void chart_update(chart_t *c, double data[]);
void chart_print(chart_t *c);

#endif
