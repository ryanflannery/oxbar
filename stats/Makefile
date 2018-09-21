# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2
LDFLAGS += -lm

OBJS  = battery.o cpu.o memory.o net.o nprocs.o volume.o stats.o
TESTS = battery.t cpu.t memory.t net.t nprocs.t volume.t

.PHONY: all clean objects tests
.DEFAULT: all

all: $(OBJS) $(TESTS)
objects: $(OBJS)
tests: $(TESTS)

.c.o:
	$(CC) $(CFLAGS) $<

.for st in $(TESTS)
${st}: ${st}.o ${st:R}.o
	$(CC) -o $@ $(LDFLAGS) ${st}.o ${st:R}.o
.endfor

clean:
	rm -f $(OBJS)
	rm -f $(TESTS)
	rm -f *.t.o
