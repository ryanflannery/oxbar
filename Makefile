# install locations
PREFIX   ?= /usr/local
BINDIR   ?= $(PREFIX)/bin
MANDIR   ?= $(PREFIX)/man/man1
SHAREDIR ?= $(PREFIX)/share/oxbar

# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm -lutil -lpthread -lm

# objects (OBJS = this dir, SOBJS = stats/*, GOBJS = giu/*, WOBJS = widgets/*)
OBJS  = settings.o widgets.o oxbar.o
SOBJS = stats/battery.o stats/cpu.o stats/memory.o stats/net.o stats/nprocs.o \
		  stats/volume.o stats/stats.o
GOBJS = gui/chart.o gui/xcore.o gui/xdraw.o gui/gui.o
WOBJS = widgets/battery.o widgets/volume.o widgets/nprocs.o widgets/memory.o \
		  widgets/cpus.o widgets/cpushort.o widgets/cpuslong.o widgets/net.o \
		  widgets/time.o widgets/util.o

.PHONY: clean install testruns cppcheck scan-build iwyu gprof loc gource

oxbar: $(OBJS) $(SOBJS) $(GOBJS) $(WOBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(SOBJS) $(GOBJS) $(WOBJS)

$(SOBJS):
	$(MAKE) -C stats $(MFLAGS) objects

$(GOBJS):
	$(MAKE) -C gui $(MFLAGS) objects

$(WOBJS):
	$(MAKE) -C widgets $(MFLAGS) objects

.c.o:
	$(CC) $(CFLAGS) $<

all: oxbar
	$(MAKE) -C stats   $(MFLAGS) $@
	$(MAKE) -C gui     $(MFLAGS) $@
	$(MAKE) -C widgets $(MFLAGS) $@

clean:
	$(MAKE) -C stats   $(MFLAGS) $@
	$(MAKE) -C gui     $(MFLAGS) $@
	$(MAKE) -C widgets $(MFLAGS) $@
	@echo make clean \(local\)
	rm -f $(OBJS)
	rm -f oxbar
	rm -f oxbar.core

install:
	$(MAKE) -C man $(MFLAGS) $@
	install oxbar $(BINDIR)
	install -d $(SHAREDIR)
	install -m 644 sample.oxbar.conf $(SHAREDIR)

# some simple test runs that work the gui/widget logic
testruns:
	@echo just sigint / ctrl-c these
	./testruns.sh
	@echo all done

# clang-analyzer (should ALWAYS be clean)
cppcheck:
	cppcheck --quiet --std=c89 -I/usr/include --enable=all --force .

# cppcheck (should ALWAYS be clean)
scan-build: clean
	scan-build --status-bugs make

# run include-what-you-use (identify includes that are missing/extra)
iwyu:
	make -k CC=include-what-you-use

# gprof / memory profiler run and visualize output
gprof: clean
	CC=gcc CFLAGS="-g -pg -fno-pie -fPIC" LDFLAGS="-g -pg -fno-pie -lc" $(MAKE)
	@echo kill with control-c when done sampling and view gprof.png
	@echo NOTE: longer runs produce tighter results / smaller chart
	./oxbar
	gprof oxbar gmon.out > gprof.analysis
	gprof2dot gprof.analysis | dot -Tpng -o gprof.png

# rebuild todo file based on all "TODO" comments in code
TODO::
	grep -nr TODO * \
		| grep -v '^TODO' \
		| grep -v '^CONTRIBUTING.md' \
		| grep -v '^Makefile' > $@

# report # of lines per flie
loc::
	@echo Lines in core oxbar > loc
	wc -l `find . -name "*.c" -a ! -name "*.d.c"` >> loc
	@echo Lines in tests and drivers >> loc
	wc -l `find . -name "*.d.c" -o -name "*.t.cc"` >> loc
	@echo Lines in build setup >> loc
	wc -l `find . -name "Makefile" -o -name "testruns.sh" -o -name "*.conf"` >> loc
	cat loc

# rebuild the architecture image showing #include dependencies
tree.png::
	cinclude2dot --paths --merge module --exclude '.t.c|.d.c' > source.dot
	dot -Tpng -Grankdir=LR -Gratio=fill source.dot > $@
	rm source.dot

# run gource on the repo / a visualization of activity over time
gource:
	gource -f -c 4 -a 1
