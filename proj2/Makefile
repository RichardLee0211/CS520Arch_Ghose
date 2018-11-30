# Compile and Link flags, libraries
CC=clang++
CFLAGS= -g -Wall -std=c++11 -c
LDFLAGS= -g -Wall -std=c++11
LIBS:=

PROGS:= apex_sim

all: $(PROGS)

# Add all object files to be linked in sequence
APEX_OBJS:= global.o cpu_base.o cpu_helper.o cpu.o file_parser.o  main.o

apex_sim: $(APEX_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@


# $(COMPILE_DEBUG)$(CC) $(CFLAGS) -c -o $@ $<
# $(COMPILE_DEBUG)echo "CC $<"

.PHONY: clean
clean:
	rm -f *.o *.d *~ $(PROGS)

# others
test: test.c
	clang -std=c11 test.c -o test

.PHONY: run
run: apex_sim
	./apex_sim test.d/input.asm > test.d/output.txt
