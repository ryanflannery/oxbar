# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -lm `pkg-config --libs pangocairo`

OBJS		= xcore.o xdraw.o histogram.o

all: $(OBJS)

.PHONY: all clean
.DEFAULT: all

.c.o:
	$(CC) -o $@ $(CFLAGS) $<

clean:
	rm -f $(OBJS)
