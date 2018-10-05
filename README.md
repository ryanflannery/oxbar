[![License: ISC](https://img.shields.io/badge/license-ISC-blue.svg "License: ISC")](LICENSE)
[![Build Status](https://travis-ci.org/ryanflannery/oxbar.svg?branch=master)](https://travis-ci.org/ryanflannery/oxbar)

# oxbar

oxbar is a small, concise system monitor that displays various stats on an X
window that's immovable and outside tabbing in most window managers. It's built
on and for OpenBSD, and is a replacement for my old
[xstatbar](https://ryanflannery.net/hacking/xstatbar/). It supports FreeType
font rendering, alpha transparency, and is highly configurable.

It's motivated mostly by the frequent complaints I get about old xstatbar and
its shortcomings. Shaming works yo.

# status

oxbar is still under early development, but is now stable and usable. There's
no documentation yet (`man` page or usable `-h` info). That's forthcoming, once
development settles.

It supports all the command line flags `xstatbar` did and much more... most
aspects of the widgets can be configured through `-S widget.name=value` flags.
If you take a look at
[settings.c](https://github.com/ryanflannery/oxbar/blob/master/settings.c)
you'll see what they all are.

Once development settles I'll focus on adding documentation and cleaning that
interface up.

# current features

   * True transparency support with a compositing window manager
     (I recommend compton in ports over xcompmgr in base)
   * FreeType fonts rendered beautifully & easily w/ pango
   * Configurable display (all colors, text, spacing, etc)
   * Left/Right/Center aligned widgets (and any combination of those)
   * Types of stats supported:
      * Battery/AC status, simple progress bar, time remaining
      * Volume level w/ progress bar (mute status forthcoming )
      * Number of processes (current total # processes only as of now)
      * Memory usage & breakdown (current only as of now)
      * CPUs usage & breakdown w/ charts
      * Current date/time (is that a status?)

# screenshots

![](images/default.png)

[View (raw) Image](images/default.png?raw=true)

# usage

See 'ol `xstatbar` for the basics of height/width and placement (or see
`settings.c`). Otherwise wait until I can document this.

## i liked xstatbar's look

You can achieve that with:
```bash
oxbar -y 0 -p 0 -f "fixed 16px" -S display.bgcolor=#000000
```

The only thing lacking is each individual cpu's states being shown. That is
forthcoming.

# outstanding stuff, prioritized

Roughly in order or my priority...

   * cleanup widget interface - remove state from the static methods and make
     them a proper component
   * SUPPORT "MUTE" STATUS IN THE VOLUME WIDGET!
   * add stats & widgets for temperature sensors
   * icons in the display could help condense it further
   * i like weather... would be nice to have a wttr widget
   * support vertical rendering (a sidebar!)
