#ifndef SPU_DBG
#define SPU_DBG

#include "Processor_definitions.h"
#include "DEBUG_PRINTF.h"

#ifndef NO_DEBUG
#define SPU_DUMP(processor, Printf_Format_Func) SPU_Dump(processor, Printf_Format_Func)
#else
#define SPU_DUMP(processor, Printf_Format_Func)
#endif

void SPU_Dump(const SPU_data * processor, void (* Printf_Format_Func)(const void * value));

#endif
