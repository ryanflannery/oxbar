# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -lm `pkg-config --libs pangocairo`

OBJS		= xcore.o xdraw.o chart.o histogram.o tseries.o

all: $(OBJS)

.PHONY: all clean
.DEFAULT: all

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OBJS)
