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
void draw_oxbar();

int
main ()
{
   settings_t settings;
   settings_load_defaults(&settings);
   g_ui = ui_create(&settings);
   stats_init();

   /* NOTE: we are now ready to update stats and draw.... */
   stats_update();   /* first stats collection */
   draw_oxbar();     /* initializes first-time widget properties */

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
      switch (xevent->response_type & ~0x80) {   /* TODO: wtf is "& ~0x80?" */
         case XCB_EXPOSE:
            draw_oxbar();
            break;
         default:
            printf("recieved some other event\n");
            break;
      }
   }

   stats_close();
   ui_destroy(g_ui);

   return 0;
}

void
signal_handler(int sig)
{
   switch (sig) {
      case SIGHUP:   /* TODO: reload config on this, once i hvae that */
      case SIGINT:
      case SIGQUIT:
      case SIGTERM:
         ui_destroy(g_ui);
         stats_close();
         exit(1);
         break;
      case SIGALRM:
         stats_update();
         draw_oxbar();
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

void
draw_oxbar()
{
   ui_clear(g_ui);

   if (BATTERY.is_setup)
      ui_widget_battery_draw(g_ui, &BATTERY);

   if (VOLUME.is_setup)
      ui_widget_volume_draw(g_ui, &VOLUME);

   if (NPROCS.is_setup)
      ui_widget_nprocs_draw(g_ui, &NPROCS);

   if (MEMORY.is_setup)
      ui_widget_memory_draw(g_ui, &MEMORY);

   if (CPUS.is_setup)
      ui_widget_cpus_draw(g_ui, &CPUS);

   ui_widget_time_draw(g_ui);

   ui_flush(g_ui);
}
