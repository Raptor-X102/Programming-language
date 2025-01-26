#include "Processor_Funcs.h"

//debug

void Printf_Format_Function(const void * value) {

    double tmp = 0;
    memcpy(&tmp, value, sizeof(double));
    DEBUG_PRINTF("%.2lg ", tmp);
}

Commands_data_struct Commands_data[] =
{

#include "Commands_data.h"

};

#undef COMMANDS_DEF
/*********************************************************** */

void SPU_Ctor(SPU_data * processor, const char* filename) {

    //const int poison = -666;
    STACK_CTOR(&(processor->stack), 10, sizeof(double));
    STACK_CTOR(&(processor->call_stack), 10, sizeof(uint64_t));
    Read_code_file(processor, filename);

    double* tmp_ptr = (double*) calloc(RAM_Size, sizeof(double));

    if(tmp_ptr) {

        processor->RAM = tmp_ptr;
        tmp_ptr = NULL;
    }
    else
        DEBUG_PRINTF("ERROR: RAM was not allocated\n");
}

void SPU_Run(SPU_data * processor) { // TODO: rename (prefix)

    //DEBUG_PRINTF("*processor = ", *processor);
    while(true) {

        if(processor->cmd_code[processor->IP] == CMD_HLT)
            break;

        else
            Commands_data[processor->cmd_code[processor->IP]].SPU_Func(processor);

        SPU_DUMP(processor, Printf_Format_Function);
    }
}

void Read_code_file(SPU_data * processor, const char* file_name) {

    FILE * file = fopen(file_name, "rb");

    if(!file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return;
    }

    char signature[10] = {};
    int version = 0;

    fread(signature, sizeof(char), 5, file);
    if(ferror(file)) {

        DEBUG_PRINTF("ERROR: fread failed\n");
        return;
    }
    DEBUG_PRINTF("%s\n", signature);

    fread(&version, sizeof(int), 1, file);
    if(ferror(file)) {

        DEBUG_PRINTF("ERROR: fread failed\n");
        return;
    }
    DEBUG_PRINTF("%d\n", version);

    fread(&(processor->code_size), sizeof(uint64_t), 1, file);
    if(ferror(file)) {

        DEBUG_PRINTF("ERROR: fread failed\n");
        return;
    }
    DEBUG_PRINTF("(processor->code_size) = %d\n", (processor->code_size));
    //getchar();

    if(processor->code_size) {

        char* tmp = (char*) calloc(processor->code_size+10, sizeof(char));

        if(tmp) {

            processor->cmd_code = tmp;
            tmp = NULL;
        }

        else {

            DEBUG_PRINTF("ERROR: memory was not allocated\n");
            return;
        }
    }

    else {

        DEBUG_PRINTF("ERROR: null size\n");
        return;
    }

    fread(processor->cmd_code, sizeof(char), processor->code_size, file);
    if(ferror(file)) {

        DEBUG_PRINTF("ERROR: fread failed\n");
        return;
    }
    DEBUG_PRINTF("read_code_file\n");
    for(int i = 0; i < processor->code_size; i++)
        DEBUG_PRINTF("%x ", processor->cmd_code[i]);

    DEBUG_PRINTF("\n");
}

void SPU_Push(SPU_data* processor) {

    double tmp = Get_push_arg(processor);
    StackPush(&processor->stack, &tmp);
}

double Get_push_arg(SPU_data* processor) {

    int option_byte = processor->cmd_code[processor->IP+1];
    processor->IP += 2;

    if(option_byte & RAM_MASK) {

        int64_t result = 0;

        if(option_byte & REGISTER_MASK) {

            result += (int64_t) processor->registers[processor->cmd_code[processor->IP]];
            processor->IP++;
        }

        if(option_byte & NUMBER_MASK) {

            int64_t temp_int = 0;
            memcpy(&temp_int, &processor->cmd_code[processor->IP], sizeof(int64_t));
            result += temp_int;
            processor->IP += sizeof(int64_t);
        }

        return (processor->RAM[result]);
    }

    else {

        double result = 0;

        if(option_byte & REGISTER_MASK) {

            result += processor->registers[processor->cmd_code[processor->IP]];
            processor->IP++;
        }

        if(option_byte & NUMBER_MASK) {

                double temp_int = 0;
                memcpy(&temp_int, &processor->cmd_code[processor->IP], sizeof(double));
                result += temp_int;
                processor->IP += sizeof(double);
            }

        return result;
    }
}

void SPU_Hlt(SPU_data* processor) {

    processor->IP++;
}

void SPU_Pop(SPU_data* processor) {

    void* tmp = Get_pop_arg(processor);
    StackPop(&processor->stack, tmp);
}

void* Get_pop_arg(SPU_data* processor) {

    int option_byte = processor->cmd_code[processor->IP+1];
    processor->IP += 2;
    int64_t result = 0;

    if(option_byte & RAM_MASK) {

        if(option_byte & REGISTER_MASK) {

            result += (int64_t)processor->registers[processor->cmd_code[processor->IP]];
            processor->IP++;
        }

        if(option_byte & NUMBER_MASK) {

            int64_t temp_int = 0;
            memcpy(&temp_int, &processor->cmd_code[processor->IP], sizeof(int64_t));
            result += temp_int;
            processor->IP += sizeof(int64_t);
        }

        return &processor->RAM[result];
    }

    else {

        processor->IP++;
        return &processor->registers[processor->cmd_code[processor->IP-1]];
    }
}

#define DEF_FUNC(name, sign)\
void SPU_##name(SPU_data* processor) {\
    double pop1 = 0;\
    double pop2 = 0;\
\
    StackPop(&(processor->stack), &pop1);\
    StackPop(&(processor->stack), &pop2);\
\
    double result = pop2 sign pop1;\
    StackPush(&(processor->stack), &result);\
\
   processor->IP++;\
\
}

#include "SPU_funcs_def_ariphm.h"

#undef DEF_FUNC

void SPU_Sqrt(SPU_data* processor) {

    double tmp_double = 0;
    StackPop(&processor->stack, &tmp_double);

    tmp_double = sqrt(tmp_double);
    StackPush(&processor->stack, &tmp_double);

    processor->IP++;
}

void SPU_Out(SPU_data* processor) {

    double out = 0;
    StackPop(&(processor->stack), &out);
    DEBUG_PRINTF("\nOUT: %lg\n\n", out);
    processor->IP++;
}

#define DEF_JMP(type, sign) \
void SPU_J##type(SPU_data* processor) {\
\
    double pop1 = 0;\
    double pop2 = 0;\
\
    StackPop(&(processor->stack), &pop1);\
    StackPop(&(processor->stack), &pop2);\
\
    if(pop1 sign pop2)\
        memcpy(&processor->IP, &processor->cmd_code[processor->IP+1], sizeof(uint64_t));\
\
    else\
        processor->IP += 1 + sizeof(uint64_t);\
}

#include "SPU_funcs_def_jumps.h"

#undef DEF_JMP

void SPU_Je(SPU_data* processor) {

    double pop1 = 0;
    double pop2 = 0;

    StackPop(&(processor->stack), &pop1);
    StackPop(&(processor->stack), &pop2);

    if(fabs(pop1 - pop2) < Epsilon)
        memcpy(&processor->IP, &processor->cmd_code[processor->IP+1], sizeof(uint64_t));

    else
        processor->IP += 1 + sizeof(uint64_t);
}

void SPU_Jne(SPU_data* processor) {

    double pop1 = 0;
    double pop2 = 0;

    StackPop(&(processor->stack), &pop1);
    StackPop(&(processor->stack), &pop2);

    if(fabs(pop1 - pop2) > Epsilon)
        memcpy(&processor->IP, &processor->cmd_code[processor->IP+1], sizeof(uint64_t));

    else
        processor->IP += 1 + sizeof(uint64_t);
}

void SPU_Jmp(SPU_data* processor) {

    memcpy(&processor->IP, &processor->cmd_code[processor->IP+1], sizeof(uint64_t));
}

void SPU_Call(SPU_data* processor) {

    uint64_t tmp_int = processor->IP + 1 + sizeof(uint64_t);
    StackPush(&processor->call_stack, &tmp_int);
    SPU_Jmp(processor);
}

void SPU_Ret(SPU_data* processor) {

    uint64_t tmp_addr = 0;
    StackPop(&processor->call_stack, &tmp_addr);
    processor->IP = tmp_addr;
}

void SPU_RAM_Draw(SPU_data* processor) {

    processor->IP++;

    uint64_t height = 0, width = 0;

    memcpy(&height, &processor->cmd_code[processor->IP], sizeof(uint64_t));
    processor->IP += sizeof(uint64_t);

    memcpy(&width, &processor->cmd_code[processor->IP], sizeof(uint64_t));
    processor->IP += sizeof(uint64_t);

    DEBUG_PRINTF("\n");

    for(uint64_t y = 0; y < height*width; y += width) {
        for(uint64_t x = y; x < y + width; x++)
            DEBUG_PRINTF("%c", (char) processor->RAM[x]);

        DEBUG_PRINTF("\n");
    }

    DEBUG_PRINTF("\n");
}
void SPU_Dtor(SPU_data* processor) {

    StackDtor(&processor->stack);
    processor->IP = 0;
    processor->code_size = 0;
    free(processor->cmd_code);
}
