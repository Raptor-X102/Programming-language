#ifndef STK_DEBUG_FUNC
#define STK_DEBUG_FUNC

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Stack_definitions.h"
#include "Stack_Functions.h"

enum Error_Code {

    Stack_Null_Pointer = 0x1,
    Data_Null_Pointer = 0x2,
    Negative_Capacity = 0x4,
    Negative_Size = 0x8,
    Capacity_Less_Size = 0x10,
    Left_Struct_Canary_Is_Dead = 0x20,
    Right_Struct_Canary_Is_Dead = 0x40,
    Left_Data_Canary_Is_Dead = 0x80,
    Right_Data_Canary_Is_Dead = 0x100,
    Wrong_Struct_Hash = 0x200,
    Wrong_Data_Hash = 0x400
};

#ifndef NO_DEBUG
    #define STACK_ASSERT(stack) StackAssert(stack, __FILE__, __func__, __LINE__)

    #define ERROR_CHECK(error_code_var, error_code) if((error_code_var & error_code) == error_code) \
                                                        fprintf(stderr, "ERROR: " #error_code "\n")
#else
    #define STACK_ASSERT(stack)
#endif

#define STACK_DUMP(stack, printf_func) Stack_Dump(stack, printf_func, __FILE__, __func__, __LINE__)
#define STACK_DUMP_FO(stack, file, printf_func) Stack_Dump_fo(stack, file, printf_func, __FILE__, __func__, __LINE__)
void Stack_Dump(const Stack_t * stack, void (*Printf_Format_Func)(const void * value),
                const char * file, const char* func, int line);
void Stack_Dump_fo(const Stack_t * stack, const char * file_name, void (* Printf_Format_Func)(const void * value, FILE* file1),
                const char * file, const char* func, int line);

#ifndef NO_DEBUG

int StackAssert (Stack_t * stack, const char * file, const char* func, int line);
int Stack_error(const Stack_t * stack);
void Error_output (int error_code,const char * name);
size_t Hash_sum(const void * buffer_start, const void * buffer_end);

#endif
#endif
