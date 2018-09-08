#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>

typedef struct memory_info {
   bool  is_setup;

   /* raw (byte) values */
   int   active, free, total;
   int   swap_used, swap_total;

   /* percentage values */
   float active_pct, free_pct, total_pct;
   float swap_used_pct;

   /* string (displayable / human readable) values */
   char *str_active;
   char *str_free;
   char *str_total;
   char *str_swap_used;
   char *str_swap_total;
} memory_info_t;

extern memory_info_t MEMORY;

void memory_init();
void memory_update();
void memory_close();

#endif
