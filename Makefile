all: lldump

# OPTDEBUGFLAGS = -O2
OPTDEBUGFLAGS = -g

CFLAGS = $(OPTDEBUGFLAGS) -MD -Wall -Werror -std=c99

-include src/*.d

lldump: lldump.o
	$(CC) $(CFLAGS) -o lldump lldump.o

clean:
	rm -f lldump lldump.o
