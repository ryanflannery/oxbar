#ifndef WIFI_H
#define WIFI_H

#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net80211/ieee80211.h>
#include <net80211/ieee80211_ioctl.h>

#include <stdbool.h>

struct wifi_info {
   bool  is_setup;
   int   socket;
   char *ifname;
	struct ieee80211_bssid bssid;
   unsigned int signal_strength;
};

extern struct wifi_info WIFI;

void wifi_init();
void wifi_update();
void wifi_close();

#endif
