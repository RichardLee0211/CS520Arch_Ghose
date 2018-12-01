/**
 * public project wide, but not public to main.c(clients)
 * there are two type of global data,
 * 1. config data, config module set and other modules read
 * 2. private global data, used for inter-module communicate, unknown for outside world
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <limits.h> // for INT_MAX
#include <inttypes.h>

typedef unsigned int uint;

/*
 * these config data should be global,
 * maybe there will be config module read config file and change these global config data.
 * other modules then read them
 */

/* wenchen regster valid bits */
#define NUM_REGS 32 /* R0~R31, require R0~R15 */
#define NUM_UREGS 40

/* valid flag status, because they are easy to initial with memset */
#define VALID 0
#define INVALID -1

/* unused value in instruction struct */
#define UNUSED_REG_INDEX -1
#define UNUSED_IMM INT_MAX

/* convert pc to code index */
#define PC_START_INDEX 4000
#define BYTES_PER_INS 4

/* status of stalled flag */
#define STALLED 1
#define UNSTALLED 0

/* status of free flag */
// #define FREE 1
// #define

/* status of busy flag */
#define BUSY 1
#define UNBUSY 0
#define BUSY_NEW 10
#define BUSY_DEFAULT 1
#define BUSY_MUL_DELAY 2
#define BUSY_MEM_DELAY 3
#define BUSY_DONE 0
#define BUSY_ALMOST_DONE 1 // if nextStage is almost done, copy stage to nextstage latch


#define DATA_MEM_SIZE 4096

#define NUM_IQ_ENTRY 16
#define NUM_ROB_ENTRY 32
#define NUM_LSQ_ENTRY 20
#define OPCODE_SIZE 8

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

extern int enable_interactive;

#endif /* GLOBAL_H */
