# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 -I/usr/X11R6/include
LDFLAGS += -lm -L/usr/X11R6/lib -lxcb -lxcb-randr

OBJS    = battery.o brightness.o cpu.o memory.o net.o nprocs.o volume.o wifi.o util.o stats.o
DRIVERS = battery.d brightness.d cpu.d memory.d net.d nprocs.d volume.d wifi.d

.PHONY: all clean objects drivers
.DEFAULT: all

all: $(OBJS) $(DRIVERS)
objects: $(OBJS)
drivers: $(DRIVERS)

.c.o:
	$(CC) $(CFLAGS) $<

.for driv in $(DRIVERS)
${driv}: ${driv}.o ${driv:R}.o
	$(CC) -o $@ $(LDFLAGS) ${driv}.o ${driv:R}.o util.o
.endfor

clean:
	rm -f $(OBJS)
	rm -f $(DRIVERS)
	rm -f *.d.o
