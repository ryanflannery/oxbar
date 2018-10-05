#include <err.h>
#include <unistd.h>
#include <pthread.h>

#include "widgets.h"
#include "settings.h"
#include "gui/gui.h"
#include "stats/stats.h"

gui_t     *gui;                     /* global gui object                      */
pthread_t  pthread_stats_updater;   /* update stats & redraw every 1 second   */
pthread_t  pthread_sig_handler;     /* listen & respond to signals (SIGKILL)  */
pthread_t  pthread_gui;             /* handle x events and redraw             */
pthread_mutex_t mutex_gui;          /* guard all calls to x11/pango/cairo     */

volatile sig_atomic_t SIG_QUIT  = 0;   /* SIGKILL/SIGQUIT/exit flag           */
volatile sig_atomic_t SIG_PAUSE = 0;   /* suspend/sleep flag                  */
volatile sig_atomic_t SIG_CONT = 0;    /* continue/wakeup flag                */

void*
thread_stats_updater()
{
   /* ignore all signals */
   sigset_t set;
   sigfillset(&set);
   if (pthread_sigmask(SIG_SETMASK, &set, NULL))
      errx(1, "%s: pthread_sigmask failed", __FUNCTION__);

   /* every 1 second, update stats and re-draw the gui */
   while (1) {
      usleep(1000000);  /* 1 second */
      if (!SIG_PAUSE) {
         pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
         stats_update();
         pthread_mutex_lock(&mutex_gui);
         gui_draw(gui);
         pthread_mutex_unlock(&mutex_gui);
         pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      }
   }
   return NULL;
}

void
signal_handler(int sig)
{
   switch (sig) {
   case SIGHUP:      /* TODO: reload config file here (once supported) */
   case SIGINT:
   case SIGQUIT:
   case SIGTERM:
      SIG_QUIT = 1;
      break;
   case SIGTSTP:
      SIG_PAUSE = 1;
      break;
   case SIGCONT:
      SIG_CONT = 1;
      break;
   }
}

void*
thread_sig_handler()
{
   /* this thread listens to these signals */
   sigset_t set;
   sigemptyset(&set);
   sigaddset(&set, SIGHUP);
   sigaddset(&set, SIGINT);
   sigaddset(&set, SIGQUIT);
   sigaddset(&set, SIGTERM);
   sigaddset(&set, SIGTSTP);
   sigaddset(&set, SIGCONT);
   if (pthread_sigmask(SIG_SETMASK, &set, NULL))
      errx(1, "%s: pthread_sigmask failed", __FUNCTION__);

   /* upon recieving these signals, execute signal_handler in this thread */
   struct sigaction sig_act;
   sig_act.sa_flags = 0;
   sig_act.sa_handler = signal_handler;
   if (sigaction(SIGHUP,  &sig_act, NULL)
   ||  sigaction(SIGINT,  &sig_act, NULL)
   ||  sigaction(SIGQUIT, &sig_act, NULL)
   ||  sigaction(SIGTERM, &sig_act, NULL)
   ||  sigaction(SIGTSTP, &sig_act, NULL)
   ||  sigaction(SIGCONT, &sig_act, NULL))
      err(1, "%s: sigaction failed", __FUNCTION__);

   /* every 1/10th of a second, check the status of that signal handler */
   while (1) {
      usleep(100000);   /* 1/10 second */
      if (SIG_CONT) {
         SIG_PAUSE = 0;
         SIG_CONT = 0;
      }
      if (SIG_QUIT) {
         if (pthread_cancel(pthread_gui)
         ||  pthread_cancel(pthread_stats_updater)
         ||  pthread_cancel(pthread_sig_handler))         /* ...it's painless */
            errx(1, "%s: pthread_cancels failed", __FUNCTION__);
      }
   }
   return NULL;
}

void*
thread_gui()
{
   /* ignore all signals */
   sigset_t set;
   sigfillset(&set);
   if (pthread_sigmask(SIG_SETMASK, &set, NULL))
      errx(1, "%s: pthread_sigmask failed", __FUNCTION__);

   /* enter x event loop (blocking & infinite) - redraw on certain events */
   xcb_generic_event_t *xevent;
   while ((xevent = xcb_wait_for_event(gui->xinfo->xcon))) {
      switch (xevent->response_type & ~0x80) {  /* TODO: why the `& ~0x80`? */
      case XCB_EXPOSE:
         pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
         pthread_mutex_lock(&mutex_gui);
         gui_draw(gui);
         pthread_mutex_unlock(&mutex_gui);
         pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
         break;
      default:
         break;
      }
   }
   return NULL;
}

int
main(int argc, char *argv[])
{
   /* init settings */
   settings_t settings;
   settings_load_defaults(&settings);
   settings_parse_cmdline(&settings, argc, argv);

   /* setup gui and stats, then do initial stats update and paint */
   stats_init();
   stats_update();
   gui = gui_init(settings.display.wmname,
         settings.display.bgcolor,
         settings.display.font,
         settings.display.x, settings.display.y,
         settings.display.w, settings.display.h,
         settings.display.padding_top);
   widgets_init(gui, &settings, &OXSTATS);
   gui_draw(gui);

   /* and we're running! start all threads */
   if (pthread_create(&pthread_sig_handler, NULL, thread_sig_handler, NULL)
   ||  pthread_create(&pthread_stats_updater, NULL, thread_stats_updater, NULL)
   ||  pthread_create(&pthread_gui, NULL, thread_gui, NULL))
      errx(1, "pthread_creates failed");

   /* wait for done (only from signal handler) */
   if (pthread_join(pthread_gui, NULL)
   ||  pthread_join(pthread_stats_updater, NULL)
   ||  pthread_join(pthread_sig_handler, NULL))
      errx(1, "pthread_joins failed");

   /* cleanup */
   stats_close();
   widgets_free();
   gui_free(gui);

   return 0;
}
