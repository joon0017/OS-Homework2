C = gcc
CFLAGS = -g -D_POSIX_C_SOURCE -Wall -std=c11

SRCS = cimin.c
OBJS = $(SRCS:.c=.o)
EXE1 = cimin

$(EXE1): $(OBJS)
	$(C) -o $@ $^
%.o: %.c
	$(C) -c $(CFLAGS) $<

.PHONY: all clean
all: $(EXE1)
clean:
	rm -f $(OBJS) $(EXE1)
