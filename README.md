# oxbar

Having a small, concise, visual indicator of numerous system stats is often
useful on a workstation for developers. While there are many options for Linux,
there are few that work well with OpenBSD natively. This project aims to offer
a solution specifically for OpenBSD.

Also it's a replacement for my old, very crappy
[xstatbar](https://ryanflannery.net/hacking/xstatbar/).

# status

This is still pretty rough, but mostly stable and usable.
I'm only publishing now to github to track my work work and share with a few
other folk on irc who seem interested.

There is much refactoring needed -- this is known. I'm still organizing key
features and really building a PoC at this point.

Note there's no `man` page yet, or `-h` flag to provide usefule help
information. That said, this is quite customizable. It supports all the
command line flags `xstatbar` did and much more... most aspects of the widgets
can be configured through `-S widget.name=value`. If you take a look at
[settings.c](../blob/master/settings.c) you'll see what they all are.

Once development settles I'll focus on adding documentation and cleaning that
interface up.

# current features

Stats currently supported are:

   * Some transparency support (i have the basics, need to tweak, see below)
   * FreeType fonts rendered beutifully & easily w/ pango
   * Configurable display (all colors, text, spacing, etc)
   * Types of stats supported:
      * Battery/AC status, simple progress bar, time remaining
      * Volume level w/ progress bar (mute status forthcoming )
      * Number of processes (current total # processes only as of now)
      * CPUs usage & breakdown w/ charts
      * Memory usage & breakdown (current only as of now)
      * Current date/time (is that a status?)

# usage

See 'ol `xstatbar` for the basics of height/width and placement (or see
`settings.c`). Otherwise wait until I can document this.

# outstanding stuff, prioritized

Roughly in order or my priority...

   * cleanup widget interface - remove state from the static methods and make
     them a proper component
   * fully support right-to-left rendering (properly)... if i do this right,
     i'll also get vertical rendering (so you could have a sidebar)
   * get alpha transparency working properly
   * DETERMINE "MUTE" STATUS FOR VOLUME
   * add stats & widgets for temperature sensors and... weather?
   * images in the display could help condense it further
