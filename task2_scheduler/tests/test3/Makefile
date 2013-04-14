CC = gcc
CFLAGS = -DNDEBUG -O3 -Wall

.PHONY: all clean

all: yield

yield: yield.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f yield
