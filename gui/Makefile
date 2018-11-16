# build & link flags (XXX remember to tweak CXX versions below)
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2
CFLAGS  += `pkg-config --cflags pangocairo`
LDFLAGS += -lm -L/usr/X11R6/lib -lxcb -lxcb-icccm
LDFLAGS += `pkg-config --libs pangocairo`

# all objects + drivers defined here
.include "Makefile.in"

.PHONY: clean objects tests drivers
.DEFAULT: all

all:     $(GUI_OBJS) $(GUI_DRIVERS)
objects: $(GUI_OBJS)
drivers: $(GUI_DRIVERS)

.c.o:
	$(CC) $(CFLAGS) $<

# drivers (system tests)
xcore.d: xcore.o xcore.d.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xcore.d.o

chart.d: chart.o chart.d.o
	$(CC) -o $@ $(LDFLAGS) chart.o chart.d.o

xdraw.d: xcore.o chart.o xdraw.o xdraw.d.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xdraw.o chart.o xdraw.d.o


# unit tests (using gtest, so c++)
CXX ?= clang++
CXXFLAGS  += -c -Wall -Wextra -Werror -O2
CXXFLAGS  += `pkg-config --cflags pangocairo`
LXXDFLAGS += -lm -L/usr/X11R6/lib -lxcb -lxcb-icccm
LXXDFLAGS += -L/usr/local/lib -lgtest -lgtest_main -lpthread
LXXDFLAGS += `pkg-config --libs pangocairo`

TESTS = xcore.t   chart.t
TOBJS = xcore.t.o chart.t.o

tests: $(TESTS)
	gtester $(TESTS)

.cc.o:
	$(CXX) $(CXXFLAGS) $<

xcore.t: xcore.t.o xcore.o
	$(CXX) -o $@ $(LXXDFLAGS) xcore.o xcore.t.o

chart.t: chart.t.o chart.o
	$(CXX) -o $@ $(LXXDFLAGS) chart.o chart.t.o

clean:
	rm -f $(GUI_OBJS)
	rm -f $(GUI_DRIVERS)
	rm -f $(TOBJS)
	rm -f $(TESTS)
