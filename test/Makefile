# Compile and Link flags, libraries
CC=g++
CFLAGS= -g -Wall -std=c++11
LDFLAGS= -g
LIBS=

PROGS= test

all: $(PROGS)

# Add all object files to be linked in sequence
OBJS:=global.o unit1.o unit2.o main.o

test: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.d *~ $(PROGS)
