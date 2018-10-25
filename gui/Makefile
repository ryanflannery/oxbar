# build flags (XXX remember to tweak CXX versions below)
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -lm -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm

OBJS = xcore.o xdraw.o chart.o gui.o
TESTS = xfont.t xdisp.t xwin.t xdraw.t chart.t xcore.t
DRIVERS = xcore.d

.PHONY: clean objects tests drivers

all: $(OBJS) $(TESTS) $(DRIVERS)
objects: $(OBJS)
tests: $(TESTS)
drivers: $(DRIVERS)

.c.o:
	$(CC) $(CFLAGS) $<

xfont.t: xcore.o xfont.t.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xfont.t.o

xdisp.t: xcore.o xdisp.t.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xdisp.t.o

xwin.t: xcore.o xwin.t.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xwin.t.o

xdraw.t: xcore.o xdraw.o chart.o xdraw.t.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xdraw.o chart.o xdraw.t.o

chart.t: chart.o chart.t.o
	$(CC) -o $@ $(LDFLAGS) chart.o chart.t.o

xcore.d: xcore.o xcore.d.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xcore.d.o

CXXFLAGS  += -c -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LXXDFLAGS += -lm -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm
LXXDFLAGS += -L/usr/lib -L/usr/local/lib -lgtest -lgtest_main -lpthread
CXX ?= clang++

xcore.t: xcore.t.o
	$(CXX) -o $@ $(LXXDFLAGS) xcore.o xcore.t.o

xcore.t.o: xcore.t.cc
	$(CXX) $(CXXFLAGS) xcore.t.cc


clean:
	rm -f $(OBJS)
	rm -f $(TESTS)
	rm -f *.t.o
