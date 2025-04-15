#ifndef SPU_DEFINES
#define SPU_DEFINES

enum machine_code {

    CMD_HLT = 0x0,
    CMD_PUSH = 0x1,
    CMD_POP = 0x2,

    CMD_ADD = 0x3,
    CMD_SUB = 0x4,
    CMD_MUL = 0x5,
    CMD_DIV = 0x6,

    CMD_OUT = 0x7,

    CMD_JA = 0x8,
    CMD_JAE = 0x9,
    CMD_JB = 0xA,
    CMD_JBE = 0xB,
    CMD_JE = 0xC,
    CMD_JNE = 0xD,
    CMD_JMP = 0xE,

    CMD_CALL = 0xF,
    CMD_RET = 0x10,

    CMD_DRAW = 0x11,

    CMD_SQRT = 0x12,

    RAX = 0xF1,
    RBX = 0xF2,
    RCX = 0xF3,
    RDX = 0xF4,
    REX = 0xF5,
    RFX = 0xF6,
    RGX = 0xF7,
    RHX = 0xF8,
    RIX = 0xF9,
    RJX = 0xFA,
    RKX = 0xFB,
    RLX = 0xFC,
    RMX = 0xFD,
    RXX = 0xFE,
    RYX = 0xFF,
    RZX = 0xF0,

    NUMBER_MASK = 0x1,
    REGISTER_MASK = 0x2,
    RAM_MASK = 0x4
};

#include "Stack.h"
const size_t Registers_amount = 17;
const uint64_t RAM_Size = 10000;
const unsigned int Command_len = 8;

struct SPU_data {

    char* cmd_code;
    size_t code_size;
    uint64_t IP;
    Stack_t stack;
    Stack_t call_stack;
    double registers[Registers_amount];
    double* RAM;
};

#endif
