#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>

struct memory_info {
   bool  is_setup;

   /* raw (byte) values */
   int   active, free, total;
   int   swap_used, swap_total;

   /* percentage values */
   float active_pct, free_pct, total_pct;
   float swap_used_pct;
};

extern struct memory_info MEMORY;

void memory_init();
void memory_update();
void memory_close();

#endif
