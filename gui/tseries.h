#ifndef TSERIES_H
#define TSERIES_H

#include <sys/types.h>

typedef struct tseries {
   size_t   size;
   size_t   current;
   double  *values;
} tseries_t ;

tseries_t* tseries_init(size_t size);
void tseries_update(tseries_t *t, double v);
void tseries_free(tseries_t *t);
void tseries_print(tseries_t *t);

#endif
