# Enables debug messages while compiling
COMPILE_DEBUG=@

# Compile and Link flags, libraries
CC=$(CROSS_PREFIX)g++
CFLAGS= -g -Wall -std=c++11
LDFLAGS=
LIBS=

PROGS= test

all: $(PROGS)

# Add all object files to be linked in sequence
OBJS:=global.o unit1.o unit2.o main.o

test: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(COMPILE_DEBUG)$(CC) $(CFLAGS) -c -o $@ $<
	$(COMPILE_DEBUG)echo "CC $<"

clean:
	rm -f *.o *.d *~ $(PROGS)
