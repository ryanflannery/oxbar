#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net80211/ieee80211.h>
#include <net80211/ieee80211_ioctl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "wifi.h"
#include "util.h"

struct wifi_info WIFI;

/*
 * TODO wifi stats collector only works on a 802.11 egress interface.
 * Is that...good though? For a laptop setup using only wifi, this works.
 * But ideally we could configure the interface!
 */

void
wifi_init()
{
   int ibssid;

   WIFI.ifname = get_egress();

   /* open an inet socket and get ready to query the bssid of it (if avail) */
	WIFI.socket = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&WIFI.bssid, 0, sizeof(WIFI.bssid));
	strlcpy(WIFI.bssid.i_name, WIFI.ifname, sizeof(WIFI.bssid.i_name));

   /* attempt to grab the bssid of egress if */
	ibssid = ioctl(WIFI.socket, SIOCG80211BSSID, &WIFI.bssid);
   if (0 == ibssid)
      WIFI.is_setup = true;   /* egress is a 802.11 if, we're good */
   else
      WIFI.is_setup = false;  /* that's not the case - bail */
}

void
wifi_update()
{
   if (!WIFI.is_setup)
      return;

   struct ieee80211_nodereq request;
   bzero(&request, sizeof(request));

   bcopy(WIFI.bssid.i_bssid, &request.nr_macaddr, sizeof(request.nr_macaddr));
   strlcpy(request.nr_ifname, WIFI.ifname, sizeof(request.nr_ifname));
   if (0 == ioctl(WIFI.socket, SIOCG80211NODE, &request) && request.nr_rssi)
         WIFI.signal_strength = IEEE80211_NODEREQ_RSSI(&request);
}

void
wifi_close()
{
   if (!WIFI.is_setup)
      return;

   close(WIFI.socket);
}
