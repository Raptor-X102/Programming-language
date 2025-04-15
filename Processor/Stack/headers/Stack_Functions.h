#ifndef STK_FUNCS
#define STK_FUNCS

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Stack_definitions.h"
#include "Stack_Debug_Functions.h"

const int Init_stack_size = 10;

#ifndef NO_DEBUG
#define STACK_CTOR(stack, capacity, element_size, poison) StackCtor(stack, capacity, element_size, poison, #stack, __FILE__,__func__, __LINE__)
#else
#define STACK_CTOR(stack, capacity, element_size) StackCtor(stack, capacity, element_size)
#endif

#ifndef NO_DEBUG
void StackCtor (Stack_t * stack, int64_t capacity, size_t element_size,
               const void * const poison_val,const char * name,
                const char * file, const char * func, int line);
#else
void StackCtor (Stack_t * stack, int64_t capacity, size_t element_size);
#endif

static bool StackResize(Stack_t * stack, const char option, ...);
void StackPush(Stack_t * stack, const void * value);
void StackPop(Stack_t * stack, void * return_var);
void StackDtor (Stack_t * stack);

#endif
