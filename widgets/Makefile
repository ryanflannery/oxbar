# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -L/usr/X11R6/lib `pkg-config --libs pangocairo` -lxcb -lxcb-icccm

OBJS  = battery.o volume.o nprocs.o memory.o cpus.o cpuslong.o net.o time.o util.o

.PHONY: all clean objects
.DEFAULT: all

all: $(OBJS)
objects: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OBJS)
