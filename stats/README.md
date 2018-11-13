# oxbar stats collectors

This directory contains all the stats collectors used by oxbar.
They each follow a pattern, that should be followed as much as possible,
described here.

## pattern to follow for all stat collectors
* Each collector of a type of stats, like cpu stats, lives in `cpu.h` and
   `cpu.c`, with full test driver in `cpu.d.c`
* Each collector declares a type like `struct cpu_stats` containing the
   display-ready versions of the stats it collects
* Any other local state the collector needs are static private locals in `cpu.c`
* The collectors themselves don't have to worry about malloc()/free()'ing
   their `struct foo_stats` types - that's done by `stats.h`/`stats.c`
* Each collector has an init, update, and free method taking a pointer to
   their respective stats object. E.g.
   * `void cpu_init(struct cpu_stats*)`
   * `void cpu_update(struct cpu_stats*)`
   * `void cpu_close(struct cpu_stats*)`
* `stats.h`/`stats.c` handle allocating and loading the stats object for every
   collector, and wrap the init, update, and close methods for all collectors

## patterns within each collector
* Each collector's stat type has a boolean member `is_setup`
* The `_init(...)` method sets that appropriately
* The `_update(...)` method checks that, and ignores updates if not setup
* The `_cleanup(...)` method can use it to aid cleanup
* For some collectors, that enablement can change over time

## expectations by oxbar on the stats components
* All stats are wrapped into the global `OXBAR` instance of `struct oxstats`
   defined in `stats.h`
* That object represents a global, read-only dump of all stats available
* History is *NOT* tracked in the collectors - widgets do that if they want
* Stats should be grouped logically by what, and how, they access data. There
   should be no relation between stats collectors and widgets. E.g. it's fine
   for a widget to use stats from multiple collectors. That's why the `OXSTATS`
   abstraction exists: to present all stats to every widget.
