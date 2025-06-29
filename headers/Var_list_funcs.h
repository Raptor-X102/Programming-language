#ifndef VAR_LIST
#define VAR_LIST

#include "Lang_funcs.h"

const int64_t Var_list_error_value = -0x404;
const size_t Var_list_init_size = 4;
const size_t Var_list_expansion_coeff = 2;
const double Var_list_init_value = 0;

bool Var_list_ctor(Var_list* var_list, size_t init_size);
int64_t Find_func_data(Func_data_list* func_list, void* variable_data);
int64_t Find_variable(Var_list* var_list, char* variable, uint64_t var_len);
int64_t Insert_var(Var_list* var_list, char* variable, double var_value, uint64_t var_len);
Variable_data* Create_variable_data(char* var_name, uint64_t var_len, double var_value, int64_t line, int64_t col);
bool Var_list_expand(Var_list* var_list);
bool Var_list_dtor(Var_list* var_list);
int64_t Insert_check_funcs(Func_data_list* func_list, const char* name);

#endif
