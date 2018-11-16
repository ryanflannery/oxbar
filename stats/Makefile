# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 -I/usr/X11R6/include
LDFLAGS += -lm -L/usr/X11R6/lib -lxcb -lxcb-randr

# all objects + drivers defined here
.include "Makefile.in"

.PHONY: all clean objects drivers
.DEFAULT: all

all:     $(STATS_OBJS) $(STATS_DRIVERS)
objects: $(STATS_OBJS)
drivers: $(STATS_DRIVERS)

.c.o:
	$(CC) $(CFLAGS) $<

.for driv in $(STATS_DRIVERS)
${driv}: ${driv}.o ${driv:R}.o
	$(CC) -o $@ $(LDFLAGS) ${driv}.o ${driv:R}.o util.o
.endfor

clean:
	rm -f $(STATS_OBJS)
	rm -f $(STATS_DRIVERS)
	rm -f *.d.o
