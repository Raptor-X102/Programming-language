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

    CMD_AX = 0x1,
    CMD_BX = 0x2,
    CMD_CX = 0x3,
    CMD_DX = 0x4,
    CMD_EX = 0x5,
    CMD_FX = 0x6,
    CMD_GX = 0x7,
    CMD_HX = 0x8,
    CMD_IX = 0x9,
    CMD_JX = 0xA,
    CMD_KX = 0xB,
    CMD_LX = 0xC,
    CMD_MX = 0xD,
    CMD_XX = 0x18,
    CMD_YX = 0x19,
    CMD_ZX = 0x1A,

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
