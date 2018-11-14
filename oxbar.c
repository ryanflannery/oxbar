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

/*
 * Various outstanding stuff that doesn't belong elsewhere...
 * TODO allow configuration of interfaces used by net & wifi collectors
 * TODO polish error messages (err/errx calls)
 * TODO make window FLOAT & ALWAYS-ON-TOP properties set-able
 */
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "widgets.h"
#include "settings.h"
#include "gui/gui.h"
#include "gui/xcore.h"
#include "stats/stats.h"

static struct xfont  *xfont = NULL;    /* loaded pango font info              */
static struct xdisp  *xdisp = NULL;    /* core x display (via xcb) info       */
static struct xwin   *xwin  = NULL;    /* oxbar's x window (xcb) info         */
static struct gui    *gui   = NULL;    /* oxbar's gui                         */
static struct settings settings;       /* global settings for oxbar           */

static pthread_t  pt_stats_updater;    /* update stats & draw every second    */
static pthread_t  pt_sig_handler;      /* listen & respond to signals         */
static pthread_t  pt_gui;              /* handle x events and redraw          */
static pthread_mutex_t mutex_gui;      /* guard all calls to gui              */

volatile sig_atomic_t SIG_QUIT   = 0;  /* SIGKILL/SIGQUIT/exit flag           */
volatile sig_atomic_t SIG_PAUSE  = 0;  /* suspend/sleep flag                  */
volatile sig_atomic_t SIG_CONT   = 0;  /* continue/wakeup flag                */
volatile sig_atomic_t SIG_RELOAD = 0;  /* SIGHUP/reload config flag           */

void
ignore_all_signals()
{
	sigset_t set;
	if (sigfillset(&set))
		err(1, "%s: sigfillset failed", __FUNCTION__);
	if (pthread_sigmask(SIG_SETMASK, &set, NULL))
		errx(1, "%s: pthread_sigmask failed", __FUNCTION__);
}

void
setup_gui()
{
	xfont = xfont_init(&settings.font);

	if (-1 == settings.window.h)
		settings.window.h = xfont->height
		    + settings.gui.padding.top + settings.gui.padding.bottom
		    + settings.gui.margin.top  + settings.gui.margin.bottom;

	if (-1 == settings.window.y)
		settings.window.y = xdisp->display_height - settings.window.h;

	if (-1 == settings.window.w)
		settings.window.w = xdisp->display_width;

	xwin = xwin_init(xdisp, &settings.window);
	gui = gui_init(xfont, xwin, &settings.gui);
	widgets_init(gui, &settings, &OXSTATS);
	gui_draw(gui);
}

void
cleanup_gui()
{
	widgets_free();
	gui_free(gui);
	xwin_free(xwin);
	xfont_free(xfont);
}

/* thread: every 1 second, update stats and re-draw the gui */
void*
thread_stats_updater()
{
	ignore_all_signals();
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

/* thread: x event loop (blocking & infinite) - redraw on certain events */
void*
thread_gui()
{
	ignore_all_signals();
	xcb_generic_event_t *xevent;
	while ((xevent = xcb_wait_for_event(xdisp->con))) {
		switch (xevent->response_type & ~0x80) {
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

void
signal_handler(int sig)
{
	switch (sig) {
	case SIGHUP:
		SIG_RELOAD = 1;
		break;
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

/* thread: just listen for signals and respond appropriately */
void*
thread_sig_handler()
{
	sigset_t set;
	if (sigemptyset(&set)
	||  sigaddset(&set, SIGHUP)
	||  sigaddset(&set, SIGINT)
	||  sigaddset(&set, SIGQUIT)
	||  sigaddset(&set, SIGTERM)
	||  sigaddset(&set, SIGTSTP)
	||  sigaddset(&set, SIGCONT))
		err(1, "%s: sigaddset failed", __FUNCTION__);
	if (pthread_sigmask(SIG_SETMASK, &set, NULL))
		errx(1, "%s: pthread_sigmask failed", __FUNCTION__);

	struct sigaction sig_act;
	sig_act.sa_flags = 0;
	sig_act.sa_handler = signal_handler;
	if (sigaction(SIGHUP,  &sig_act, NULL)
	||  sigaction(SIGINT,  &sig_act, NULL)
	||  sigaction(SIGQUIT, &sig_act, NULL)
	||  sigaction(SIGTERM, &sig_act, NULL)
	||  sigaction(SIGTSTP, &sig_act, NULL)
	||  sigaction(SIGCONT, &sig_act, NULL))
		err(1, "%s: failed to install signal handlers", __FUNCTION__);

	while (1) {
		usleep(100000);   /* 1/10 second */
		if (SIG_RELOAD) {
			pthread_mutex_lock(&mutex_gui);
			settings_reload_config(&settings);
			cleanup_gui();
			setup_gui();
			pthread_mutex_unlock(&mutex_gui);
			SIG_RELOAD = 0;
		}
		if (SIG_CONT) {
			SIG_PAUSE = 0;
			SIG_CONT = 0;
		}
		if (SIG_QUIT) {
			if (pthread_cancel(pt_gui)
			||  pthread_cancel(pt_stats_updater)
			||  pthread_cancel(pt_sig_handler))
				errx(1, "failed to cancel ongoing pthreads");
		}
	}
	return NULL;
}

int
main(int argc, char *argv[])
{
	stats_init();
	settings_init(&settings, argc, argv);
	xdisp = xdisp_init();
	setup_gui();

	/*
	* XXX Note pledge(2) will not work for oxbar :(
	* All the ioctl(2) calls and what-not in stats/ prohibit that.
	*/

	/* and we're running! start all threads */
	if (pthread_create(&pt_sig_handler, NULL, thread_sig_handler, NULL)
	||  pthread_create(&pt_stats_updater, NULL, thread_stats_updater, NULL)
	||  pthread_create(&pt_gui, NULL, thread_gui, NULL))
		errx(1, "failed to create pthreads");

	/* wait for done (only from signal handler) */
	if (pthread_join(pt_gui, NULL)
	||  pthread_join(pt_stats_updater, NULL)
	||  pthread_join(pt_sig_handler, NULL))
		errx(1, "failed to join pthreads");

	/* cleanup */
	cleanup_gui();
	xdisp_free(xdisp);
	stats_close();

	return 0;
}
