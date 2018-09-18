#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "gui.h"
#include "settings.h"
#include "stats/stats.h"

oxbarui_t *g_ui;

void signal_handler(int sig);
void setup_timer();

int
main(int argc, char *argv[])
{
   settings_t settings;

   settings_load_defaults(&settings);
   settings_parse_cmdline(&settings, argc, argv);

   g_ui = ui_init(&settings);
   stats_init();

   /* NOTE: we are now ready to update stats and draw.... */
   stats_update();   /* first stats collection */
   ui_draw(g_ui);     /* initializes first-time widget properties */

   signal(SIGHUP,  signal_handler);
   signal(SIGINT,  signal_handler);
   signal(SIGQUIT, signal_handler);
   signal(SIGTERM, signal_handler);

   /* this starts updating stats AND drawing oxbar every 1 second */
   setup_timer();

   /*
    * this starts the xcb loop waiting for expose events to redraw, which
    * occurs anytime the window is covered and then re-exposed
    */
   xcb_generic_event_t *xevent;
   while ((xevent = xcb_wait_for_event(g_ui->xinfo->xcon))) {
      /* TODO WTF is "& ~0x80?" needed for in this xcb_event_t check?
       * This is straight from xcb documentation. I have no idea why it's
       * needed and haven't figured out yet.
       */
      switch (xevent->response_type & ~0x80) {
         case XCB_EXPOSE:
            ui_draw(g_ui);
            break;
         default:
            /* TODO Identify what the other X events are we recieve */
            break;
      }
   }

   stats_close();
   ui_free(g_ui);

   return 0;
}

/* TODO Fix XCB event handling and signal handling
 * I have some issues below - namely in a signal handler I'm calling non-
 * reentrant safe code (stats_update() and ui_draw()).
 * That's horrible...and I know it.
 *
 * I need to rectify using xcb_wait_for_event(..) for my main X loop (a
 * blocking API) with my use of setitimer(2) for the 1-second updates
 * -or-
 * Go full pthread?
 * -or-
 * sem_poll(2) below?
 * -or-
 *  Something else...?
 */
void
signal_handler(int sig)
{
   switch (sig) {
      /* TODO On SIGHUP, reload config instead of quiting */
      case SIGHUP:
      case SIGINT:
      case SIGQUIT:
      case SIGTERM:
         /* TODO Bad signal handler! Bad! These aren't reentrant!
          * See todo above this function.
          */
         ui_free(g_ui);
         stats_close();
         exit(1);
         break;
      case SIGALRM:
         /* TODO Bad signal handler! Bad! These aren't reentrant!
          * See todo above this function.
          */
         stats_update();
         ui_draw(g_ui);
         break;
   }
}

void
setup_timer()
{
   struct sigaction sig_act;
   struct itimerval timer;

   /* create timer signal handler */
   if (0 > sigemptyset(&sig_act.sa_mask))
      err(1, "%s: sigemptyset failed", __FUNCTION__);

   sig_act.sa_flags = 0;
   sig_act.sa_handler = signal_handler;
   if (0 > sigaction(SIGALRM, &sig_act, NULL))
      err(1, "%s: sigaction failed", __FUNCTION__);

   /* setup timer details */
   timer.it_value.tv_sec = 1;       /* wait 1 second for first timer fire */
   timer.it_value.tv_usec = 0;
   timer.it_interval.tv_sec = 1;    /* wait 1 second for subsequent fires */
   timer.it_interval.tv_usec = 0;

   /* register timer */
   if (0 > setitimer(ITIMER_REAL, &timer, NULL))
      err(1, "%s: setitimer failed", __FUNCTION__);
}

