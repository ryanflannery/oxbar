# build flags (XXX remember to tweak CXX versions below)
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -lm -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm

OBJS 	  = xcore.o chart.o xdraw.o gui.o
TESTS   = xcore.t chart.t xdraw.t
DRIVERS = xcore.d chart.d

.PHONY: clean objects tests drivers

all: $(OBJS) $(TESTS) $(DRIVERS)
objects: $(OBJS)
tests: $(TESTS)
drivers: $(DRIVERS)

.c.o:
	$(CC) $(CFLAGS) $<

# tests (OLD - TODO migrating these to below set of unit tests)
xdraw.t: xcore.o xdraw.o chart.o xdraw.t.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xdraw.o chart.o xdraw.t.o

# drivers (system tests)
xcore.d: xcore.o xcore.d.o
	$(CC) -o $@ $(LDFLAGS) xcore.o xcore.d.o

chart.d: chart.o chart.d.o
	$(CC) -o $@ $(LDFLAGS) chart.o chart.d.o

# unit tests
CXXFLAGS  += -c -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LXXDFLAGS += -lm -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm
LXXDFLAGS += -L/usr/lib -L/usr/local/lib -lgtest -lgtest_main -lpthread
CXX ?= clang++

xcore.t: xcore.t.o xcore.o
	$(CXX) -o $@ $(LXXDFLAGS) xcore.o xcore.t.o

xcore.t.o: xcore.t.cc
	$(CXX) $(CXXFLAGS) xcore.t.cc

chart.t: chart.t.o chart.o
	$(CXX) -o $@ $(LXXDFLAGS) chart.o chart.t.o

chart.t.o: chart.t.cc
	$(CXX) $(CXXFLAGS) chart.t.cc


clean:
	rm -f $(OBJS)
	rm -f $(TESTS)
	rm -f $(DRIVERS)
	rm -f *.t.o
