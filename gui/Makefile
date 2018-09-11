# build flags
CFLAGS	+= -c -std=c89 -Wall -Wextra -Werror -O2 -I/usr/X11R6/include -I/usr/local/include
LDFLAGS	+= -lm -L/usr/X11R6/lib -L/usr/local/lib -lxcb -lxcb-icccm -lcairo

OBJS		= xcore.o xdraw.o histogram.o

all: $(OBJS)

.PHONY: all clean
.DEFAULT: all

.c.o:
	$(CC) -o $@ $(CFLAGS) $<

clean:
	rm -f $(OBJS)
