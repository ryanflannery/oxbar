# Status

Note: This is very rough right now. I'm only publishing now to github to track
my work work and share with a few other folk on irc who seem interested.
You're welcome to try it out, but it's very unstable, unconfigurable, and
rough at this point. Collaboration welcomed.

# Motivation

Having a small, concise, visual indicator of numerous system stats is often
useful on a workstation for developers. While there are many options for Linux,
there are few that work well with OpenBSD natively. This project aims to offer
a solution specifically for OpenBSD.

# Goals

Proivde a native, well supported, X11 stats bar for OpenBSD with that can
**quickly and succinctly** summarize key stats of a workstation that a
developer may be interested in. It **should not aim to answer every question**,
but rather provide a clean, easy-to-read visualization of possible issues and
where to investigate further.

It should support configuration/knobs where appropriate (say
appearance) and up-to-date, thorough, documentation is mandatory.

# Samples & Features

Stats currently supported are:

   * Battery/AC status 
   * Volume level (mute status forthcoming - need to dive into mixerctl(2))
   * Number of processes (current total # processes only as of now)
   * CPUs usage & breakdown (ONLY idle/cpu supported now, have other stats but
     not plotted yet)
   * Memory usage & breakdown (current only as of now)

# Usage

No configuration options yet - only in code. This will likely be added last,
after I determine how best to query and display all information.

# Plan / TODO

Roughly in order or my priority...

   * Add plots of historical memory & cpu (support historical plot generically)
   * Support showing date/time
   * Determine "mute" status in volume (stats/volume.c)...this has proven
     challenging
   * Support full pango font detection/configuration
   * Support configuration of the various options & aesthetics
   * Add module: support named hw.sensor mibs and their values in the display
   * Add module: netowrk usage & breakdown (with history)
   * Add module: PF usage & breakdown (with history)
   * Use images for stats (like volume/battery/memory/etc)...scaling & licensing
     here are an issue

