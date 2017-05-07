.PHONY: all
all: srv cli

# OPTDEBUGFLAGS = -O2
OPTDEBUGFLAGS = -g

CFLAGS = -std=c99 $(OPTDEBUGFLAGS) -MD -Wall -Werror

-include *.d

srv: srv.o lib.o
	$(CC) $^ -o $@

cli: cli.o lib.o
	$(CC) $^ -o $@

clean:
	rm -f srv cli *.o
