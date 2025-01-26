#ifndef SPU_FUNCS
#define SPU_FUNCS

#include <math.h>
#include "Processor_definitions.h"
#include "Processor_debug.h"
#include "DEBUG_PRINTF.h"

void SPU_Ctor(SPU_data * processor, const char* filename);
void SPU_Run(SPU_data * processor);
void Read_code_file(SPU_data * processor, const char* file_name);

void SPU_Hlt(SPU_data * processor);

void SPU_Push(SPU_data* processor);
double Get_push_arg(SPU_data* processor);

void SPU_Pop(SPU_data* processor);
void* Get_pop_arg(SPU_data* processor);

void SPU_Add(SPU_data* processor);
void SPU_Sub(SPU_data* processor);
void SPU_Mul(SPU_data* processor);
void SPU_Div(SPU_data* processor);
void SPU_Sqrt(SPU_data* processor);

void SPU_Out(SPU_data* processor);

void SPU_Ja(SPU_data* processor);
void SPU_Jae(SPU_data* processor);
void SPU_Jb(SPU_data* processor);
void SPU_Jbe(SPU_data* processor);
void SPU_Je(SPU_data* processor);
void SPU_Jne(SPU_data* processor);
void SPU_Jmp(SPU_data* processor);

void SPU_Call(SPU_data* processor);
void SPU_Ret(SPU_data* processor);

void SPU_RAM_Draw(SPU_data* processor);

void SPU_Dtor(SPU_data* processor);

#define COMMANDS_DEF(name, cmd, arg_n, spu_func, asm_func) name, cmd, arg_n, spu_func

struct Commands_data_struct {

    char command_name[Command_len];
    int command_num;
    int args_amount;
    void (*SPU_Func) (SPU_data * processor);
};

void Printf_Format_Function(const void * value);

#endif
