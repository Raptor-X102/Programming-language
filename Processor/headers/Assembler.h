#ifndef ASM
#define ASM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Processor_definitions.h"
#include "GetFileSize2.h"
#include "DEBUG_PRINTF.h"

const size_t Labels_amount = 50;
const size_t Cmd_name_length = 32;
const size_t signature_len = 8;

struct labels_data {

    uint64_t label_pos;
    char label_name[Cmd_name_length];
};

struct Labels {

    labels_data* labels_arr;
    size_t labels_count;
    size_t labels_amount;
};

struct Commands_data {

    char command_name[Command_len];
    char command_num;
    int args_amount;
    void (*Asm_Func) (char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels);
};

void Compile_code(const char* compiling_file, const char* output_file, Labels* labels);
//char * Compiled_filename(const char* compiling_file);
uint64_t Convert_txt_to_code(FILE* input_file, FILE* output_file, uint64_t code_length, Labels* labels);
void ASM_dump(const char* code_data, uint64_t curr_code_byte);
void Labels_Ctor(Labels* labels, size_t labels_amount);
void Labels_Dtor(Labels* labels);
void Insert_label(Labels* labels, const char* label_name, size_t label_pos);
int64_t Find_label(const char* label_name, Labels* labels);
//bool Check_jmp_func(const char* prev_cmd);
char Check_register(const char* code_str);
bool Chech_brackets(const char* code_str);
void ASM_Get_arg(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels);
void ASM_Void(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels);
void ASM_Get_label(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels);
void Label_handle(char* code_data, uint64_t* curr_code_byte, const char* code_str, Labels* labels);
void Get_next_word(char* input_buffer, uint64_t* curr_pos, uint64_t* inword_pos, uint64_t code_length);

#ifdef NO_DEBUG
#define ASM_DUMP(code_data, curr_code_byte) ASM_dump(code_data, curr_code_byte)
#else
#define ASM_DUMP(code_data, curr_code_byte)
#endif

#endif
