#include <err.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

#include "net.h"

int
main()
{
   net_init();
   if (!NET.is_setup)
      errx(1, "failed to setup core!");

   printf("egress interface is: '%s'\n", NET.iface);
   printf("%10s %10s %10s %10s %10s %10s %10s %10s\n",
         "#p in", "#p out", "#new in", "#new out",
         "#b in", "#b out", "#b new in", "#b new out");

   while (1)
   {
      net_update();
      printf("%10lu %10lu %10lu %10lu %10llu %10llu %10llu %10llu\n",
            NET.raw_ip_packets_in, NET.raw_ip_packets_out,
            NET.new_ip_packets_in, NET.new_ip_packets_out,
            NET.raw_bytes_in, NET.raw_bytes_out,
            NET.new_bytes_in, NET.new_bytes_out);
      sleep(1);
   }

   printf("%0f\n", roundf(0.0)); /* FIXME fml why i need this */

   net_close();
}