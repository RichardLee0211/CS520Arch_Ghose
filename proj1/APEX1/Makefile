# Enables debug messages while compiling
COMPILE_DEBUG=@

# Compile and Link flags, libraries
CC=$(CROSS_PREFIX)gcc
CFLAGS= -g -Wall
LDFLAGS=
LIBS=

PROGS= apex_sim

all: $(PROGS)

# Add all object files to be linked in sequence
APEX_OBJS:=file_parser.o cpu.o main.o

apex_sim: $(APEX_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(COMPILE_DEBUG)$(CC) $(CFLAGS) -c -o $@ $<
	$(COMPILE_DEBUG)echo "CC $<"

clean:
	rm -f *.o *.d *~ $(PROGS)

test: test.c
	clang -std=c11 test.c -o test

run: apex_sim
	./apex_sim test.d/input.asm > test.d/output.txt

run2: apex_sim
	./apex_sim test.d/input2.asm > test.d/output2.txt

run3: apex_sim
	./apex_sim test.d/input3.asm > test.d/output3.txt

run3: apex_sim
	./apex_sim test.d/input4.asm > test.d/output4.txt
