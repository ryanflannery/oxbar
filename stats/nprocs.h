#ifndef NPROCS_H
#define NPROCS_H

#include <stdbool.h>

typedef struct nprocs_info {
   bool  is_setup;

   int   nprocs;     /* total # of processes */

} nprocs_info_t;

extern nprocs_info_t NPROCS;

void nprocs_init();
void nprocs_update();
void nprocs_close();

#endif
