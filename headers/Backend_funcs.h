#ifndef BACKEND_FUNCS
#define BACKEND_FUNCS

#include "Lang_funcs.h"

#define PRINTOUT(...) fprintf(output_file, __VA_ARGS__)

bool Compile_to_ASM(const char* out_file, Node* root, Func_data_list* func_list);
void Compile_user_function_def(FILE* output_file, Node* root, Func_data_list* func_list);
void Compile_operation(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                       int64_t* if_count, int64_t* while_count);
void Compile_while(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                   int64_t* if_count, int64_t* while_count);
void Compile_if(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                int64_t* if_count, int64_t* while_count);
void Compile_return(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index);
void Compile_condition(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index);
void Compile_assignment(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index);
void Compile_operator(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index);
void Compile_user_function(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index);
void Compile_push_parameters(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index);

#endif
