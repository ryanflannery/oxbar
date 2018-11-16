# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2
CFLAGS  += `pkg-config --cflags pangocairo`
LDFLAGS += -L/usr/X11R6/lib -lxcb -lxcb-icccm
LDFLAGS += `pkg-config --libs pangocairo`

# all inputs defined here
.include "Makefile.in"

.PHONY: all clean objects
.DEFAULT: all

all:     $(WIDGET_OBJS)
objects: $(WIDGET_OBJS)

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(WIDGET_OBJS)
