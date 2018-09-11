# build flags
CFLAGS+=-c -std=c89 -Wall -Wextra -Werror -O2
#CFLAGS+=-c -std=c89 -Wall -Wextra -Werror -O2 -I/usr/X11R6/include -I/usr/X11R6/include/freetype2
#LDFLAGS+=-L/usr/X11R6/lib -lX11 -lXext -lXrender -lXau -lXdmcp -lm -lXft -lXrandr
LDFLAGS+=-lm

OBJS=battery.o cpu.o memory.o nprocs.o volume.o stats.o
TESTS=battery.t cpu.t memory.t nprocs.t volume.t

stats: $(OBJS) $(TESTS)

.DEFAULT: stats

.c.o:
	$(CC) -o $@ $(CFLAGS) $<

battery.t: battery.t.o battery.o
	$(CC) -o $@ $(LDFLAGS) battery.t.o battery.o

cpu.t: cpu.t.o cpu.o
	$(CC) -o $@ $(LDFLAGS) cpu.t.o cpu.o

memory.t: memory.t.o memory.o
	$(CC) -o $@ $(LDFLAGS) memory.t.o memory.o

nprocs.t: nprocs.t.o nprocs.o
	$(CC) -o $@ $(LDFLAGS) nprocs.t.o nprocs.o

volume.t: volume.t.o volume.o
	$(CC) -o $@ $(LDFLAGS) volume.t.o volume.o

clean:
	rm -f $(OBJS)
	rm -f $(TESTS)
	rm -f *.t.o
