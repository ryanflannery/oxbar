# Status

Note: This is very rough right now. I'm only publishing now to github to track
my work work and share with a few other folk on irc who seem interested.
You're welcome to try it out, but it's very unstable, unconfigurable, and
rough at this point. Collaboration welcomed.

There is much refactoring needed -- this is known. I'm still organizing key
features and really building a PoC at this point.

Most importantly, there is still no user configuration or customization (even
on the command line). This is coming, the refactoring is gearing towards that
(see how all core config options are explicit at the start of `main()` in
`oxbar.c`). This simply isn't a priority now, as much of this logic is
expected to change in coming refactorings.

# Motivation

Having a small, concise, visual indicator of numerous system stats is often
useful on a workstation for developers. While there are many options for Linux,
there are few that work well with OpenBSD natively. This project aims to offer
a solution specifically for OpenBSD.

# Samples & Features

Stats currently supported are:

   * Full transparency support
   * FreeType fonts rendered beutifully & easily w/ pango
   * Configurable display (all colors, text, spacing, etc)
   * Status supported:
      * Battery/AC status, simple progress bar, time remaining
      * Volume level w/ progress bar (mute status forthcoming )
      * Number of processes (current total # processes only as of now)
      * CPUs usage & breakdown w/ charts
      * Memory usage & breakdown (current only as of now)

# Usage

No configuration options yet - only in code. This will likely be added last,
after I determine how best to query and display all information.

# Plan / Priorities

Roughly in order or my priority...

   * So much code cleanup...
   * Cleanup histograms / make colors easier to configure and default prettier
   * Support showing date/time
   * Determine "mute" status in volume (stats/volume.c)...this has proven
     challenging
   * Support full pango font detection/configuration
   * Support configuration of the various options & aesthetics
   * Add module: support named hw.sensor mibs and their values in the display
   * Add module: netowrk usage & breakdown, say #bytes in/out (with history)
   * Add module: PF usage & breakdown, say #blocked in/out (with history)
   * Use images for stats (like volume/battery/memory/etc)...scaling &
     licensing here are an issue

