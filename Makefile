CC=gcc
CFLAGS=-I./include -Wall -Werror
DEPS=vmcrl.h test.h

all: vmcrl test

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

vmcrl: vmcrl.o
	$(CC) -o $@ $^

test: test.o
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm *.o
	rm vmcrl
	rm test
