#ifndef GLOBAL_H
#define GLOBAL_H

/**
 * there are two type of global data,
 * 1. config data, config module set and other modules read
 * 2. private global data, used for inter-module communicate, unknown for outside world
 */

/*
 * these config data should be global,
 * maybe there will be config module read config file and change these global config data.
 * other modules then read them
 */
typedef unsigned int uint;

/* wenchen regster valid bits */
#define NUM_REGS 32 /* R0~R31, require R0~R15 */
#define VALID 0

/* unused value in instruction struct */
#define UNUSED_REG_INDEX -1
#define UNUSED_IMM INT_MAX

/* convert pc to code index */
#define PC_START_INDEX 4000
#define BYTES_PER_INS 4

/* the state of stage in cpu */
#define STALLED 1
#define UNSTALLED 0
#define BUSY 1
#define UNBUSY 0

#define DATA_MEM_SIZE 4096

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1
// #define FORWARD_ENABLE 1
//

extern int enable_interactive;


#endif /* GLOBAL_H */
