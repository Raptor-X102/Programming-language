#ifndef CDEF
#define CDEF

COMMANDS_DEF({ "HLT",  .command_num = CMD_HLT,  .args_amount = 0, SPU_Hlt,      ASM_Void)},
COMMANDS_DEF({ "PUSH", .command_num = CMD_PUSH, .args_amount = 3, SPU_Push,     ASM_Get_arg)},
COMMANDS_DEF({ "POP",  .command_num = CMD_POP,  .args_amount = 3, SPU_Pop,      ASM_Get_arg)},
COMMANDS_DEF({ "ADD",  .command_num = CMD_ADD,  .args_amount = 0, SPU_Add,      ASM_Void)},
COMMANDS_DEF({ "SUB",  .command_num = CMD_SUB,  .args_amount = 0, SPU_Sub,      ASM_Void)},
COMMANDS_DEF({ "MUL",  .command_num = CMD_MUL,  .args_amount = 0, SPU_Mul,      ASM_Void)},
COMMANDS_DEF({ "DIV",  .command_num = CMD_DIV,  .args_amount = 0, SPU_Div,      ASM_Void)},
COMMANDS_DEF({ "OUT",  .command_num = CMD_OUT,  .args_amount = 0, SPU_Out,      ASM_Void)},
COMMANDS_DEF({ "JA",   .command_num = CMD_JA,   .args_amount = 1, SPU_Ja,       ASM_Get_label)},
COMMANDS_DEF({ "JAE",  .command_num = CMD_JAE,  .args_amount = 1, SPU_Jae,      ASM_Get_label)},
COMMANDS_DEF({ "JB",   .command_num = CMD_JB,   .args_amount = 1, SPU_Jb,       ASM_Get_label)},
COMMANDS_DEF({ "JBE",  .command_num = CMD_JBE,  .args_amount = 1, SPU_Jbe,      ASM_Get_label)},
COMMANDS_DEF({ "JE",   .command_num = CMD_JE,   .args_amount = 1, SPU_Je,       ASM_Get_label)},
COMMANDS_DEF({ "JNE",  .command_num = CMD_JNE,  .args_amount = 1, SPU_Jne,      ASM_Get_label)},
COMMANDS_DEF({ "JMP",  .command_num = CMD_JMP,  .args_amount = 1, SPU_Jmp,      ASM_Get_label)},
COMMANDS_DEF({ "CALL", .command_num = CMD_CALL, .args_amount = 1, SPU_Call,     ASM_Get_label)},
COMMANDS_DEF({ "RET",  .command_num = CMD_RET,  .args_amount = 0, SPU_Ret,      ASM_Void)},
COMMANDS_DEF({ "DRAW", .command_num = CMD_DRAW, .args_amount = 0, SPU_RAM_Draw, ASM_Void)},
COMMANDS_DEF({ "SQRT", .command_num = CMD_SQRT, .args_amount = 0, SPU_Sqrt,     ASM_Void)}

#endif
