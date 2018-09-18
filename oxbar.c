#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#include "gui.h"
#include "settings.h"
#include "stats/stats.h"

oxbarui_t *gui;
pthread_t  pthread_gui;
pthread_t  pthread_stats_updater;
pthread_t  pthread_sig_handler;

void*
thread_gui()
{
   sigset_t set;
   sigfillset(&set);
   pthread_sigmask(SIG_SETMASK, &set, NULL);  /* TODO check */

   xcb_generic_event_t *xevent;
   while ((xevent = xcb_wait_for_event(gui->xinfo->xcon))) {
      /* TODO WTF is "& ~0x80?" needed for in this xcb_event_t check?
       * This is straight from xcb documentation. I have no idea why it's
       * needed and haven't figured out yet.
       */
      switch (xevent->response_type & ~0x80) {
      case XCB_EXPOSE:
         ui_draw(gui);
         break;
      default:
         break;
      }
   }
   return NULL;
}

void*
thread_stats_updater()
{
   sigset_t set;
   sigfillset(&set);
   pthread_sigmask(SIG_SETMASK, &set, NULL);  /* TODO check */

   while (1) {
      usleep(1000000);  /* 1 second */
      stats_update();
      ui_draw(gui);
   }
   return NULL;
}

volatile sig_atomic_t SIG_QUIT = 0;

void
signal_handler(int sig)
{
   switch (sig) {
   case SIGHUP:
   case SIGINT:
   case SIGTERM:
   case SIGQUIT:
      SIG_QUIT = 1;
      break;
   }
}

void*
thread_sig_handler()
{
   sigset_t set;
   sigemptyset(&set);
   sigaddset(&set, SIGHUP);
   sigaddset(&set, SIGINT);
   sigaddset(&set, SIGTERM);
   sigaddset(&set, SIGQUIT);
   pthread_sigmask(SIG_SETMASK, &set, NULL);  /* TODO check */

   struct sigaction sig_act;
   sig_act.sa_flags = 0;
   sig_act.sa_handler = signal_handler;
   if (sigaction(SIGHUP,  &sig_act, NULL)
    || sigaction(SIGINT,  &sig_act, NULL)
    || sigaction(SIGTERM, &sig_act, NULL)
    || sigaction(SIGQUIT, &sig_act, NULL))
      err(1, "%s: sigaction failed", __FUNCTION__);

   while (1) {
      usleep(100000);  /* 1/10 second */
      if (SIG_QUIT) {
         pthread_cancel(pthread_gui);
         pthread_cancel(pthread_stats_updater);
         pthread_cancel(pthread_sig_handler);   /* ...it's painless */
      }
   }
   return NULL;
}

int
main(int argc, char *argv[])
{
   settings_t settings;
   settings_load_defaults(&settings);
   settings_parse_cmdline(&settings, argc, argv);

   gui = ui_init(&settings);
   stats_init();

   /* we are now ready to update stats and draw / do first of each */
   stats_update();
   ui_draw(gui);

   /* TODO checks */
   pthread_create(&pthread_gui, NULL, thread_gui, NULL);
   pthread_create(&pthread_stats_updater, NULL, thread_stats_updater, NULL);
   pthread_create(&pthread_sig_handler, NULL, thread_sig_handler, NULL);

   /* TODO checks */
   pthread_join(pthread_gui, NULL);
   pthread_join(pthread_stats_updater, NULL);
   pthread_join(pthread_sig_handler, NULL);

   stats_close();
   ui_free(gui);

   return 0;
}
