#include "Var_list_funcs.h"

bool Var_list_ctor(Var_list* var_list, size_t init_size) {

    Variable_data* tmp_ptr = (Variable_data*) calloc(init_size, sizeof(Variable_data));
    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    var_list->var_list = tmp_ptr;
    var_list->free_var = 0;
    var_list->var_list_size = init_size;
    var_list->status = true;

    return true;
}

int64_t Find_func_data(Func_data_list* func_list, void* variable_data) {

    Variable_data* var_data = NULL;
    memcpy(&var_data, variable_data, sizeof(Variable_data*));

    for(int index = 0; index < func_list->free_element; index++) {

        if(var_data->var_len == func_list->func_data[index].function->var_len &&
           !strncmp(func_list->func_data[index].function->var_name, var_data->var_name, var_data->var_len))
            return index;
    }

    return -1;
}

int64_t Insert_check_funcs(Func_data_list* func_list, const char* name) {

    int64_t index = Var_list_error_value;
    if (!func_list || !name) {

        DEBUG_PRINTF("ERROR: null ptr\n");
        return index;
    }

    if (func_list->free_element >= func_list->size) {

        DEBUG_PRINTF("ERROR: func list out of range\n");
        return index;
    }

    Variable_data* var_data = (Variable_data*)calloc(1, sizeof(Variable_data));
    if (!var_data) {

        DEBUG_PRINTF("ERROR in func 'Insert_check_funcs': memory was not allocated\n");
        return index;
    }

    var_data->var_name = strdup(name);
    if (!var_data->var_name) {

        DEBUG_PRINTF("ERROR: string was not copied\n");
        free(var_data);
        return index;
    }

    var_data->var_len = strlen(name);

    Func_data func_data = {};
    func_data.function = var_data;
    index = func_list->free_element;
    func_list->func_data[index] = func_data;
    func_list->free_element++;

    return index;
}

bool Var_list_dtor(Var_list* var_list) {

    if(var_list && var_list->var_list) {

        free(var_list->var_list);
        var_list->status = false;
        return true;
    }

    else
        return false;
}

int64_t Find_variable(Var_list* var_list, char* variable, uint64_t var_len) {

    if(var_list->status) {

        for(int64_t index = 0; index < var_list->free_var; index++) {

            if(!strncmp(var_list->var_list[index].var_name, variable, var_len)) {

                return index;
            }
        }
    }

    return -1;
}

Variable_data* Create_variable_data(char* var_name, uint64_t var_len, double var_value, int64_t line, int64_t col) {

    Variable_data* var_data = (Variable_data*) calloc(1, sizeof(Variable_data));
    if(!var_data) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return NULL;
    }

    var_data->var_name = var_name;
    var_data->var_len = var_len;
    var_data->var_value = var_value;
    var_data->line = line;
    var_data->col = col;

    return var_data;
}

int64_t Insert_var(Var_list* var_list, char* variable, double var_value, uint64_t var_len) {

    int64_t index = Find_variable(var_list, variable, var_len);
    if(index >= 0) {

        var_list->var_list[index].var_value = var_value;
    }

    else {


        if(var_list->free_var >= var_list->var_list_size)
            if(!Var_list_expand(var_list))
                return Var_list_error_value;

        var_list->var_list[var_list->free_var].var_value = var_value;
        var_list->var_list[var_list->free_var].var_name = variable;
        var_list->var_list[var_list->free_var].var_len = var_len;
        index = var_list->free_var;
        var_list->free_var++;
    }

    return index;
}

bool Var_list_expand(Var_list* var_list) {

    size_t prev_size = var_list->var_list_size;
    var_list->var_list_size = prev_size * Var_list_expansion_coeff;

    Variable_data* tmp_ptr = (Variable_data*) realloc(var_list->var_list,
                                                      var_list->var_list_size *
                                                      sizeof(Variable_data));
    if(!tmp_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n\n");
        return false;
    }

    memset(tmp_ptr + prev_size, 0, (var_list->var_list_size - prev_size) * sizeof(Variable_data));
    var_list->var_list = tmp_ptr;
    return true;
}
