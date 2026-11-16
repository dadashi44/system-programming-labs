CC     ?= cc
CFLAGS ?= -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE

all: create remove

create: create.c
	$(CC) $(CFLAGS) -o $@ $<

remove: remove.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f create remove

.PHONY: all clean
