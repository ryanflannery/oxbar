#ifndef XDRAW_H
#define XDRAW_H

#include "xcore.h"
#include "tseries.h"
#include "histogram.h"

/*
 * Clear the entire display / paint color everywhere AND start a new buffer
 * of drawing (needs xdraw_flush() call below to render everything after this)
 *
 * NOTE: This is part of the double buffering - it should be called at the
 * beginning of each "draw loop" (one full run through of the display) and then
 * all operations are "flushed" to the actual output when, and only when,
 * xdraw_flush() below is called.
 */
void xdraw_clear_all(xinfo_t *xinfo, const char *color);

/*
 * Flush all buffered drawing to the actual UI
 */
void xdraw_flush(xinfo_t *xinfo);

uint32_t
xdraw_printf(
      xinfo_t    *xinfo,
      const char *color,
      double      x,
      double      y,
      const char *fmt,
      ...);

uint32_t
xdraw_text_right_aligned(
      xinfo_t    *xinfo,
      const char *color,
      double      x,
      double      y,
      const char *text);

void
xdraw_hline(
      xinfo_t    *xinfo,
      const char *color,
      double      width,
      double      x1,
      double      x2);

uint32_t
xdraw_vertical_stack(
      xinfo_t     *xinfo,
      uint32_t     x,
      double       width,
      size_t       nvalues,
      const char **colors,
      double      *percents);

uint32_t
xdraw_series(
      xinfo_t       *xinfo,
      double         x,
      const char   **colors,
      tseries_t     *t);

uint32_t
xdraw_histogram(
      xinfo_t       *xinfo,
      double         x,
      const char   **colors,
      histogram_t   *h);

void hex2rgba(const char *s, double *r, double *g, double *b, double *a);

#endif
