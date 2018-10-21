# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -lm -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm

OBJS = xcore.o xdraw.o chart.o gui.o
TESTS = xfont.t xdisp.t xwin.t xdraw.t chart.t

.PHONY: clean objects tests
.DEFAULT: objects

all: $(OBJS) $(TESTS)
objects: $(OBJS)
tests: $(TESTS)

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

clean:
	rm -f $(OBJS)
	rm -f $(TESTS)
	rm -f *.t.o
