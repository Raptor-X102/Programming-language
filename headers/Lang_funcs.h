#ifndef LANG_FUNCS
#define LANG_FUNCS

#include <math.h>
#include "Lang_definitions.h"
#include "Tree_funcs.h"
#include "Binary_tree_graph_debug.h"
#include "Assembler.h"
#include "Processor_Funcs.h"

const size_t Mem_blocks_amount = 1;
const int Power_buffer_size = 12; // INT_MAX digits = 10
const int Decimal_number_system = 10;

struct Buffer_data {

    char* buffer;
    int64_t buffer_size;
};

struct Variable_data {

    char* var_name;
    uint64_t var_len;
    double var_value;
    int64_t line;
    int64_t col;
    int64_t RAM_index;
};

struct Var_list {

    Variable_data* var_list;
    size_t var_list_size;
    size_t free_var;
    bool status;
};

struct Func_data {

    Variable_data* function;
    Var_list parameters;
    Var_list local_vars;
    int64_t func_memory;
    int32_t func_rel32_address;
};

struct Func_data_list {

    Func_data* func_data;
    int64_t size;
    int64_t free_element;
    int64_t vars_total;
};

const int Error_value = -0x404;
Node* Lang_new_node(int node_type, const void* node_value, size_t node_val_size,
                    Node* left_node, Node* right_node);
void Print_node_data(const void* data, FILE* file);
void Lang_dtor(Node* root);

#define _NUM(number) Lang_new_node(NUM, &number, sizeof(double), NULL, NULL)

#define _VAR(variable) Lang_new_node(VAR, &variable, sizeof(Variable_data*), NULL, NULL)
#define _FUNC(func_name) Lang_new_node(FUNCTION, &func_name, sizeof(Variable_data*), NULL, NULL)
#define _X Lang_new_node(VAR, &X_var, sizeof(int64_t), NULL, NULL)

#define _ADD(left_node, right_node) Lang_new_node(OP, &ADD, sizeof(int64_t), left_node, right_node)
#define _SUB(left_node, right_node) Lang_new_node(OP, &SUB, sizeof(int64_t), left_node, right_node)
#define _MUL(left_node, right_node) Lang_new_node(OP, &MUL, sizeof(int64_t), left_node, right_node)
#define _DIV(left_node, right_node) Lang_new_node(OP, &DIV, sizeof(int64_t), left_node, right_node)
#define _POW(left_node, right_node) Lang_new_node(OP, &POW, sizeof(int64_t), left_node, right_node)
#define _LOG(left_node, right_node) Lang_new_node(OP, &LOG, sizeof(int64_t), left_node, right_node)

#define _LN(left_node)   Lang_new_node(OP, &LN, sizeof(int64_t), left_node, NULL)
#define _SH(left_node)   Lang_new_node(OP, &SH, sizeof(int64_t), left_node, NULL)
#define _CH(left_node)   Lang_new_node(OP, &CH, sizeof(int64_t), left_node, NULL)
#define _TH(left_node)   Lang_new_node(OP, &TH, sizeof(int64_t), left_node, NULL)

#define _SIN(left_node)  Lang_new_node(OP, &SIN, sizeof(int64_t), left_node, NULL)
#define _COS(left_node)  Lang_new_node(OP, &COS, sizeof(int64_t), left_node, NULL)
#define _TAN(left_node)  Lang_new_node(OP, &TAN, sizeof(int64_t), left_node, NULL)
#define _COT(left_node)  Lang_new_node(OP, &COT, sizeof(int64_t), left_node, NULL)
#define _EXP(left_node)  Lang_new_node(OP, &EXP, sizeof(int64_t), left_node, NULL)
#define _CTH(left_node)  Lang_new_node(OP, &CTH, sizeof(int64_t), left_node, NULL)
#define _ASH(left_node)  Lang_new_node(OP, &ASH, sizeof(int64_t), left_node, NULL)
#define _ACH(left_node)  Lang_new_node(OP, &ACH, sizeof(int64_t), left_node, NULL)
#define _ATH(left_node)  Lang_new_node(OP, &ATH, sizeof(int64_t), left_node, NULL)
#define _ABS(left_node)  Lang_new_node(OP, &ABS, sizeof(int64_t), left_node, NULL)

#define _ASIN(left_node) Lang_new_node(OP, &ASIN, sizeof(int64_t), left_node, NULL)
#define _ACOS(left_node) Lang_new_node(OP, &ACOS, sizeof(int64_t), left_node, NULL)
#define _ATAN(left_node) Lang_new_node(OP, &ATAN, sizeof(int64_t), left_node, NULL)
#define _ACOT(left_node) Lang_new_node(OP, &ACOT, sizeof(int64_t), left_node, NULL)
#define _ACTH(left_node) Lang_new_node(OP, &ACTH, sizeof(int64_t), left_node, NULL)
#define _SQRT(left_node) Lang_new_node(OP, &SQRT, sizeof(int64_t), left_node, NULL)

#define _IF(left_node) Lang_new_node(KEY_WORD, &IF, sizeof(int64_t), left_node, NULL)
#define _WHILE(left_node) Lang_new_node(KEY_WORD, &WHILE, sizeof(int64_t), left_node, NULL)

#define _IF_BRANCHES(left_node, right_node) Lang_new_node(IF_BRANCHES, &NULL_VALUE, sizeof(int64_t), left_node, right_node)
#include "Var_list_funcs.h"
#include "Lang_read_funcs.h"
#include "Backend_funcs_x64.h"
#include "Backend_funcs_nasm.h"
#include "Backend_funcs.h"
#endif
