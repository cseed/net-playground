.PHONY: all
all: lldump lltx

# OPTDEBUGFLAGS = -O2
OPTDEBUGFLAGS = -g

CFLAGS = -std=c99 $(OPTDEBUGFLAGS) -MD -Wall -Werror

-include *.d

lldump: lldump.o
	$(CC) $^ -o $@

lltx: lltx.o
	$(CC) $^ -o $@

clean:
	rm -f lldump lltx
	rm -f *.o
