#include <net/if.h>
#include <net/if_dl.h>
#include <sys/ioctl.h>

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char*
get_egress()
{
	struct ifgroupreq	 ifgr;
	struct ifg_req		*ifg;
	unsigned int		 len;
   int ioctlfd;
   char *name;

	memset(&ifgr, 0, sizeof(ifgr));
   strlcpy(ifgr.ifgr_name, "egress", sizeof(ifgr.ifgr_name));

   if ((ioctlfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      err(1, "%s: socket(AF_INET, SOCK_DGRAM) failed", __FUNCTION__);

   if (ioctl(ioctlfd, SIOCGIFGMEMB, (caddr_t)&ifgr) == -1)
      err(1, "%s: ioctl for SIOCGIFGMEMB failed", __FUNCTION__);

   len = ifgr.ifgr_len;
   if (NULL == (ifgr.ifgr_groups = calloc(1, len)))
      err(1, "%s: calloc failed (len = %d)", __FUNCTION__, len);

   if (ioctl(ioctlfd, SIOCGIFGMEMB, (caddr_t)&ifgr) == -1)
      err(1, "%s: ioctl for SIOCGIFGMEMB (2nd time) failed", __FUNCTION__);

   name = NULL;
   for (ifg = ifgr.ifgr_groups; ifg && len >= sizeof(*ifg); ifg++) {
      len -= sizeof(*ifg);
      if (name != NULL)
         errx(1, "more than 1 interface in group egress (unsupported)");
      if (NULL == (name = strdup(ifg->ifgrq_member)))
         errx(1, "%s: strdup failed for '%s'", __FUNCTION__, ifg->ifgrq_member);
   }

   free(ifgr.ifgr_groups);
   return name;
}

