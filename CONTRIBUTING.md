# guidelines for contributing

Pull requests can be made to master.
All code should follow the existing style:

1. strict c89 that's `-Wall -Wextra -Werror` clean
2. nothing found by clang analyzer (`make scan-build`) or cppcheck
   (`make cppcheck`)
3. it's ok if it's not perfect -- works-in-progress welcome
4. but it should be stable
5. and any planned/known TODOs should be document (and update `make TODO`)
6. if it introduces knobs, those should be documented
7. HAH! just kidding on that last one - there is no usage documentation (yet)

# a note about overall architecture

These are the following components

## oxbar

The `main()`, which does little more than orchestrate the various threads and
signal handlers. There are 3 threads:
1. The main timer loop - sleeps for a second, wakes up, collects stats, and
   redraws the screen
2. The signal handler - just listens to signals and sets flags -- could
   possibly merge into another thread
3. The gui loop handler - just handles xcb events (expose only currently, so
   the gui can redraw whenever it's exposed, and not wait up to 1 second)

In the main, it basically init's the other components (stats, gui, widgets),
the above threads, and then runs. After exit, it cleans everything up.

## settings

The settings component is really just a pad of all client knobs and ways to
set them (via cli flags, and eventually a config file). This is used heavily
by the widgets components as this is where all their display settings live.

## stats (subdirectory)

This is where all the stats collectors live. One module for one stats source.
E.g. `stats/cpu.*` collects all cpu information. There is a "wrapper" module
in `stats/stats.*` that wraps all the other collectors into a single
`struct` and interface, used extensively by the widget components to retrieve
the stats they display.

**NOTE:** There is currently a one-to-one relationship between stats collectors
and widgets. That is **not** a design enforcement - widgets are free to pull
from any and all stats components (e.g. you could have a widget that renders
memory and cpu information more compactly).

## gui (subdirectory)

This is where all the UI rendering logic lives. Each module has a well defined
purpose and that should be maintained. They are:

1. `gui/xcore.*` All xcb/cairo/pango setup and teardown logic lives here. It
   simply creates a window, sets the various window properties (there are
   many) and then tears it down.
2. `gui/xdraw.*` This defines a "drawing context" type called `xctx_t` that
   all drawing takes place on. It also provides a number of primitive drawing
   routines used by the widgets, such as `xdraw_printf()`,
   `xdraw_progressbar()`, `xdraw_chart()`, etc. The goal of this interface is
   to wrap all the common/usual drawing routines by the widgets and make them
   as streamlined as possible.
3. `gui/chart.*` This component holds the chart object used to hold and render
   historical data.
4. `gui/gui.*` This is an "orchestrator of widgets" .. it is responsible for
   containing widgets, mapped to "regions" of the display (left, center, and
   right-aligned stacks), and then rendering them in order. That is all.
   Ideally it maintains and manages no state.

## widgets (module + subdirectory)

This `widgets.*` component in the root directory defines an interface used by
the individual widgets defined in the `widgets/` subdirectory. This is a
polymorphic design. The `widgets.*` component itself defines that interface
and the routines to create all widgets, from recipes, add them to a gui, and
track them for teardown on exit.

TBD - document this widget piece more once it settles.
