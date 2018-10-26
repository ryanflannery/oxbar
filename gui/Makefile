# build flags (XXX remember to tweak CXX versions below)
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -lm -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm

OBJS 	  = xcore.o   chart.o   xdraw.o   gui.o
DRIVERS = xcore.d   chart.d   xdraw.d
DOBJS	  = xcore.d.o chart.d.o xdraw.d.o

.PHONY: clean objects tests drivers

all: $(OBJS) $(TESTS) $(DRIVERS)
objects: $(OBJS)
drivers: $(DRIVERS)

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
CXXFLAGS  += -c -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LXXDFLAGS += -lm -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm
LXXDFLAGS += -L/usr/lib -L/usr/local/lib -lgtest -lgtest_main -lpthread

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
	rm -f $(OBJS)
	rm -f $(DOBJS)
	rm -f $(DRIVERS)
	rm -f $(TOBJS)
	rm -f $(TESTS)
