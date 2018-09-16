# install locations
BINDIR = /usr/local/bin
MANDIR = /usr/local/man/man1

# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm

# object sets (OBJS = this dir, SOBJS = stats/*, GOBJS = giu/*)
OBJS  = gui.o oxbar.o settings.o
SOBJS = stats/battery.o stats/cpu.o stats/memory.o stats/net.o stats/nprocs.o stats/volume.o stats/stats.o
GOBJS = gui/histogram.o gui/xcore.o gui/xdraw.o gui/tseries.o

# by default, recurse into stats/ and gui/ and then build oxbar
.DEFAULT:
	$(MAKE) -C stats $(MFLAGS) $@
	$(MAKE) -C gui   $(MFLAGS) $@

all: .DEFAULT oxbar

# boilerplate
.c.o:
	$(CC) $(CFLAGS) $<

oxbar: $(OBJS) $(SOBS) $(GOBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(SOBJS) $(GOBJS)

.PHONY: clean TODO

clean:
	$(MAKE) -C stats $(MFLAGS) $@
	$(MAKE) -C gui   $(MFLAGS) $@
	rm -f $(OBJS)
	rm -f oxbar
	rm -f oxbar.core

TODO:
	grep -nr TODO * \
		| sed 's/ *\/\* *TODO/TODO/' \
		| sed 's/ *\*\///' \
		| sed 's/ *\# *TODO/TODO/' \
		| grep -v '^Makefile' > $@

cppcheck:
	cppcheck --std=c89 --enable=all . > /dev/null

scan-build:
	make clean
	scan-build make

# TODO Get gprof Profiling Working
# I'd really love to inspect and optimize my stats update and render loops
# to try and make oxbar snappier!
profile: clean
	CFLAGS="-fno-pie -g -pg" LDFLAGS="-g -pg -fno-pie -lc_p" $(MAKE)
