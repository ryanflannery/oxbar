# install locations
BINDIR	= /usr/local/bin
MANDIR	= /usr/local/man/man1

# build flags
CFLAGS	+= -c -std=c89 -Wall -Wextra -Werror -O2 -I/usr/X11R6/include -I/usr/local/include
#LDFLAGS	+= -L/usr/X11R6/lib -lX11 -lXext -lXrender -lXau -lXdmcp -lm -lXft -lXrandr
LDFLAGS	+= -L/usr/X11R6/lib -L/usr/local/lib -lxcb -lxcb-icccm -lcairo

OBJS=histogram.o xcore.o xdraw.o ui.o oxbar.o
SOBS=stats/battery.o stats/cpu.o stats/memory.o stats/nprocs.o stats/volume.o stats/stats.o

.c.o:
	$(CC) $(CFLAGS) $<

oxbar: $(OBJS) $(SOBS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(SOBS)

clean:
	rm -f $(OBJS)
	rm -f oxbar
	rm -f oxbar.core
