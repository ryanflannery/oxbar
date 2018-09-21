# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2 `pkg-config --cflags pangocairo`
LDFLAGS += -lm `pkg-config --libs pangocairo`

OBJS = xcore.o xdraw.o chart.o

.PHONY: clean objects
.DEFAULT: objects

objects: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OBJS)
