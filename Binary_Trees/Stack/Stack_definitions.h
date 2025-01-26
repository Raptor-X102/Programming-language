#ifndef STACK_DEF
#define STACK_DEF

#include <stdio.h>
#include <stdint.h>

#ifndef NO_DEBUG
    #define DEBUG_ON(...) __VA_ARGS__
    #ifndef NO_CANARY_PROTECTION
        #define CANARY(...) __VA_ARGS__
    #else
        #define CANARY(...)
    #endif

    #ifndef NO_HASH_PROTECTION
        #define HASH(...) __VA_ARGS__
    #else
        #define HASH(...)
    #endif

    #ifdef NO_ASSERT
        #define NDEBUG
        #define ABORT_SKIP(...)
    #else
        #define ABORT_SKIP(...) __VA_ARGS__
    #endif
#else
    #define NO_HASH_PROTECTION
    #define NO_CANARY_PROTECTION
    #define DEBUG_ON(...)
    #define CANARY(...)
    #define HASH(...)
    #define ABORT_SKIP(...)
#endif

#ifndef NO_CANARY_PROTECTION
const uint64_t Data_left_protector = (0xB1E55ED);
const uint64_t Data_right_protector = (0xC0DE);
const uint64_t Struct_left_protector = (0xD15AB1ED);
const uint64_t Struct_right_protector = (0x1DEA);
#endif

const double Epsilon = 1e-12;

struct Stack_t {

    #ifndef NO_CANARY_PROTECTION
    uint64_t left_struct_canary;
    #endif

    #ifndef NO_DEBUG
    const char* name;
    const char* file;
    const char* func;
    int line;
    #endif

    void * data;
    size_t element_size;
    const void * poison_val;
    char* free_data;
    int64_t size;
    int64_t capacity;
    bool Stack_Status;
    size_t mem_size_aligned;

    #ifndef NO_HASH_PROTECTION
    size_t struct_hash_sum;
    size_t data_hash_sum;
    #endif

    #ifndef NO_CANARY_PROTECTION
    uint64_t right_struct_canary;
    #endif
};
#endif
