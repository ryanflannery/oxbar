#ifndef NPROCS_H
#define NPROCS_H

#include <stdbool.h>

struct nprocs_info {
   bool  is_setup;

   int   nprocs;     /* total # of processes */
};

extern struct nprocs_info NPROCS;

void nprocs_init();
void nprocs_update();
void nprocs_close();

#endif
