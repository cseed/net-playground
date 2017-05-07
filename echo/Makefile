
.PHONY: all

all: srv cli

CFLAGS = -MD -std=c99 -Wall -Werror

srv: srv.o lib.o
	$(CC) $^ -o $@

cli: cli.o lib.o
	$(CC) $^ -o $@

clean:
	rm -f srv cli *.o
