#include "Processor_debug.h"

void SPU_Dump(const SPU_data * processor, void (* Printf_Format_Func)(const void * value)) {

    int scale = 8;
    DEBUG_PRINTF("-----------------------------------------------------------------------------------------\n");

    DEBUG_PRINTF("code:  ");
    for(int commands_amount = 0; commands_amount < processor->code_size; commands_amount += scale) {
        if(commands_amount >= scale)
            DEBUG_PRINTF("\n       ");
        for(int i = commands_amount; i < commands_amount + scale && i < processor->code_size; i++)
            DEBUG_PRINTF("%8X ", i);
        DEBUG_PRINTF("\n");
        DEBUG_PRINTF("       ");
        for(int i = commands_amount; i < commands_amount + scale && i < processor->code_size; i++) {

            DEBUG_PRINTF("%8X ", processor->cmd_code[i]);
        }
        DEBUG_PRINTF("\n");
        if(processor->IP >= commands_amount && processor->IP < commands_amount + scale) {
            DEBUG_PRINTF("              ");
            for(int k = commands_amount; k < processor->IP; k++) {

                DEBUG_PRINTF("         ");
            }

            DEBUG_PRINTF("^ IP = %2llX\n", processor->IP);
        }
    }

    DEBUG_PRINTF("Stack: ");

    for(size_t commands_amount = 0; commands_amount < processor->stack.size; commands_amount++) {

        Printf_Format_Func((char*)processor->stack.data+commands_amount*processor->stack.element_size);
    }

    DEBUG_PRINTF("\nCall Stack: ");

    for(size_t commands_amount = 0; commands_amount < processor->call_stack.size; commands_amount++) {

        uint64_t tmp_int = 0;
        memcpy(&tmp_int,
               (char*)processor->call_stack.data+
               commands_amount*processor->call_stack.element_size,
               sizeof(uint64_t));
        DEBUG_PRINTF("%llX ", tmp_int);
    }

    DEBUG_PRINTF("\nRegisters: ");

    for(int commands_amount = 0; commands_amount < Registers_amount; commands_amount++)
        DEBUG_PRINTF("%.2lg ", processor->registers[commands_amount]);

    /*DEBUG_PRINTF("\nRAM: \n");

    int scale_ram = 20;

    for(int i = 0; i < 400; i+=scale_ram) {
        for(int j = i; j < i + scale_ram; j++)
            DEBUG_PRINTF("%4.2lg ", processor->RAM[j]);

        DEBUG_PRINTF("\n");
    }
    */
    DEBUG_PRINTF("\n-----------------------------------------------------------------------------------------\n");
    getchar();
}
