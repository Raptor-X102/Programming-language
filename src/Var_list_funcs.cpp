#include "Var_list_funcs.h"

bool Var_list_ctor(Var_list* var_list, size_t init_size) {

    DEBUG_PRINTF("VL 0\n");
    Variable_data* tmp_ptr = (Variable_data*) calloc(init_size, sizeof(Variable_data));
    DEBUG_PRINTF("VL 1\n");
    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }
    DEBUG_PRINTF("VL 2\n");
    var_list->var_list = tmp_ptr;
    var_list->free_var = 0;
    var_list->var_list_size = init_size;
    DEBUG_PRINTF("VL 3\n");
    return true;
}

int64_t Find_func_data(Func_data_list* func_list, void* variable_data) {

    Variable_data* var_data = NULL;
    memcpy(&var_data, variable_data, sizeof(Variable_data*));
    DEBUG_PRINTF("func = ");
    DEBUG_PRINTF("%.*s\n", (size_t) var_data->var_len,
                                    var_data->var_name);
    for(int index = 0; index < func_list->free_element; index++) {

        DEBUG_PRINTF("%d ", index);
        DEBUG_PRINTF("%.*s\n", (size_t) func_list->func_data[index].function->var_len,
                                        func_list->func_data[index].function->var_name);
        if(var_data->var_len == func_list->func_data[index].function->var_len &&
        !strncmp(func_list->func_data[index].function->var_name, var_data->var_name, var_data->var_len))
            return index;
    }

    return -1;
}

bool Var_list_dtor(Var_list* var_list) {

    if(var_list && var_list->var_list) {

        free(var_list->var_list);
        return true;
    }

    else
        return false;
}

int64_t Find_variable(Var_list* var_list, char* variable, uint64_t var_len) {

    for(int64_t index = 0; index < var_list->free_var; index++) {

        DEBUG_PRINTF("INDEX = %d\n", index);
        DEBUG_PRINTF("var_list->free_var = %d\n", var_list->free_var);
        DEBUG_PRINTF("variable = %p\n",variable);
        DEBUG_PRINTF("var_len = %d\n",var_len);
        DEBUG_PRINTF("variable = %.*s\n", (size_t) var_len, variable);
        if(!strncmp(var_list->var_list[index].var_name, variable, var_len))
            return index;
    }

    return -1;
}

int64_t Insert_var(Var_list* var_list, char* variable, double var_value, uint64_t var_len) {

    DEBUG_PRINTF("IN INSERTION %p\n", variable);

    int64_t index = Find_variable(var_list, variable, var_len);
    DEBUG_PRINTF("INSERTION 1\n");
    if(index >= 0)
        var_list->var_list[index].var_value = var_value;

    else {

        DEBUG_PRINTF("INSERTION 2\n");
        var_list->var_list[var_list->free_var].var_value = var_value;
        var_list->var_list[var_list->free_var].var_name = variable;
        var_list->var_list[var_list->free_var].var_len = var_len;

        DEBUG_PRINTF("INSERTION 3\n");
        DEBUG_PRINTF("var_list->var_list[index].var_name = %p\n", var_list->var_list[var_list->free_var].var_name);
        DEBUG_PRINTF("var_len = %d\n", var_list->var_list[var_list->free_var].var_len);
        DEBUG_PRINTF("variable (c) = %c\n", *(var_list->var_list[var_list->free_var].var_name));
        DEBUG_PRINTF("variable = %.*s\n", (size_t) var_list->var_list[var_list->free_var].var_len,
                                                   var_list->var_list[var_list->free_var].var_name);
        index = var_list->free_var;
        var_list->free_var++;

        if(var_list->free_var >= var_list->var_list_size)
            if(!Var_list_expand(var_list))
                return Var_list_error_value;

        DEBUG_PRINTF("var_list->var_list[index].var_name = %p\n", var_list->var_list[var_list->free_var-1].var_name);
        DEBUG_PRINTF("var_len = %d\n", var_list->var_list[var_list->free_var-1].var_len);
        DEBUG_PRINTF("variable (c) = %c\n", *(var_list->var_list[var_list->free_var-1].var_name));
        DEBUG_PRINTF("variable = %.*s\n", (size_t) var_list->var_list[var_list->free_var-1].var_len,
                                                   var_list->var_list[var_list->free_var-1].var_name);

    }

    return index;
}

bool Var_list_expand(Var_list* var_list) {

    //var_list->free_var = var_list->var_list_size;
    var_list->var_list_size = var_list->var_list_size*
                              Var_list_expansion_coeff;

    Variable_data* tmp_ptr = (Variable_data*) realloc(var_list->var_list,
                                                      var_list->var_list_size *
                                                      sizeof(Variable_data));

    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    var_list->var_list = tmp_ptr;
    return true;
}
