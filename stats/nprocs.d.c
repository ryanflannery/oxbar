/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "nprocs.h"

volatile sig_atomic_t sig_stop = 0;
void stop(int __attribute__((unused)) sig) { sig_stop = 1; }

int
main()
{
   struct nprocs_stats s;
   signal(SIGINT, stop);

   nprocs_init(&s);
   if (!s.is_setup)
      errx(1, "failed to setup core!");

   printf("%8s\n", "procs");

   while (!sig_stop) {
      nprocs_update(&s);
      printf("%8d\n", s.nprocs);
      sleep(1);
   }

   nprocs_close(&s);
}
