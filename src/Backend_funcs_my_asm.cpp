#include "Backend_funcs.h"

bool Compile_to_ASM(const char* out_file, Node* root, Func_data_list* func_list) {

    FILE* output_file = fopen(out_file, "wb");
    if(!output_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }

    PRINTOUT("PUSH BX\n");
    PRINTOUT("CALL main\n");
    PRINTOUT("HLT\n\n");

    Variable_data main = {};
    main.var_name = "main";
    main.var_len = strlen(main.var_name);
    Variable_data* main_ptr = &main;
    int64_t index = Find_func_data(func_list, &main_ptr);
    if(index == -1) {

        DEBUG_PRINTF("ERROR: 'main' function was not found\n");
        return false;
    }

    if(!root) {

        DEBUG_PRINTF("ERROR: root null ptr\n");
        return false;
    }

    if(!Compile_user_function_def(output_file, root, func_list))
        return false;

    fclose(output_file);
    return true;
}

bool Compile_user_function_def(FILE* output_file, Node* root, Func_data_list* func_list) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == ANGLE_BRACKET_CL && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node)
            if(!Compile_user_function_def(output_file, root->left_node, func_list))
                return false;

        if(root->right_node)
            if(!Compile_user_function_def(output_file, root->right_node, func_list))
                return false;
    }

    else {

        int64_t index = Find_func_data(func_list, &tmp_data->value);
        if(index == -1) {

            DEBUG_PRINTF("ERROR: this function was not inserted in func_data");
            return false;
        }

        PRINTOUT("%.*s:\n", (size_t) func_list->func_data[index].function->var_len,
                                     func_list->func_data[index].function->var_name);
        if(func_list->func_data[index].parameters.free_var > 0)
            for(int i = func_list->func_data[index].parameters.free_var - 1; i >= 0; i--)
                PRINTOUT("POP [BX+%d]\n", i);

        int64_t if_count = 0, while_count = 0;
        if(root->right_node)
            if(!Compile_operation(output_file, root->right_node, func_list, index, &if_count, &while_count))
                return false;
    }

    return true;
}

bool Compile_operation(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                       int64_t* if_count, int64_t* while_count) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == TAB && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node) {

            if(!Compile_operation(output_file, root->left_node, func_list, index, if_count, while_count))
                return false;
        }

        if(root->right_node) {

            if(!Compile_operation(output_file, root->right_node, func_list, index, if_count, while_count))
                return false;
        }
    }

    else {

        if(tmp_data->value == IF && tmp_data->expression_type == KEY_WORD) {

            if(root->left_node)
                if(!Compile_condition(output_file, root->left_node, func_list, index))
                    return false;

            if(root->right_node) {

                if(!Compile_if(output_file, root->right_node, func_list, index, if_count, while_count))
                    return false;
            }
        }

        else if(tmp_data->value == WHILE && tmp_data->expression_type == KEY_WORD) {

            if(!Compile_while(output_file, root, func_list, index, if_count, while_count))
                return false;
        }

        else if(tmp_data->value == ASSIGNMENT && tmp_data->expression_type == SPECIAL_SYMBOL) {

            if(!Compile_assignment(output_file, root, func_list, index))
                return false;
        }

        else if(tmp_data->expression_type == FUNCTION) {

            if(!Compile_user_function(output_file, root, func_list, index))
                return false;
        }

        else if(tmp_data->value == RETURN && tmp_data->expression_type == KEY_WORD) {

            if(!Compile_return(output_file, root, func_list, index))
                return false;
        }

        else {

            DEBUG_PRINTF("ERROR: unknown operation\n");
            return false;
        }
    }

    return true;
}

bool Compile_while(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                   int64_t* if_count, int64_t* while_count) {

    int64_t var_len = func_list->func_data[index].function->var_len;
    char* var_name = func_list->func_data[index].function->var_name;

    int64_t while_count_copy = *while_count;
    (*while_count)++;
    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_check_cond:\n", while_count_copy);

    if(root->left_node)
        if(!Compile_condition(output_file, root->left_node, func_list, index))
            return false;

    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes\n", while_count_copy);

    PRINTOUT("JMP %.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_no\n\n", while_count_copy);

    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes:\n", while_count_copy);

    if(root->right_node)
        if(!Compile_operation(output_file, root->right_node, func_list, index, if_count, while_count))
            return false;

    PRINTOUT("JMP %.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_check_cond\n", while_count_copy);

    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_no:\n\n", while_count_copy);

    return true;
}

bool Compile_if(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                int64_t* if_count, int64_t* while_count) {

    int64_t var_len = func_list->func_data[index].function->var_len;
    char* var_name = func_list->func_data[index].function->var_name;

    int64_t if_count_copy = *if_count;
    (*if_count)++;

    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes\n", if_count_copy);

    PRINTOUT("JMP %.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_no\n\n", if_count_copy);

    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes:\n", if_count_copy);

    if(root->left_node)
        if(!Compile_operation(output_file, root->left_node, func_list, index, if_count, while_count))
            return false;

    PRINTOUT("JMP %.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes_ret\n", if_count_copy);

    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_no:\n", if_count_copy);

    if(root->right_node)
        if(!Compile_operation(output_file, root->right_node, func_list, index, if_count, while_count))
            return false;

    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes_ret:\n", if_count_copy);

    return true;
}

bool Compile_return(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    if(root->right_node)
        if(!Compile_operator(output_file, root->right_node, func_list, index))
            return false;

    PRINTOUT("POP RX\n");
    PRINTOUT("POP BX\n");
    PRINTOUT("PUSH RX\n");
    PRINTOUT("RET\n");

    return true;
}

bool Compile_condition(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    if(root->left_node)
        if(!Compile_operator(output_file, root->left_node, func_list, index))
            return false;

    if(root->right_node)
        if(!Compile_operator(output_file, root->right_node, func_list, index))
            return false;

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));

    switch(tmp_data->value) {

        #define CONDITION(value, symbol, spu_code) case value:\
                                                       PRINTOUT(spu_code " ");\
                                                       break;

        #include "Conditions_code_gen.h"
        #undef CONDITION

        default:
            DEBUG_PRINTF("ERROR: no such condition\n");
            return false;
    }

    return true;
}

bool Compile_assignment(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* node_data_left = NULL;
    Variable_data* var_data = NULL;

    if(!Compile_operator(output_file, root->right_node, func_list, index))
        return false;

    memcpy(&node_data_left, &root->left_node->node_data, sizeof(Node_data*));
    memcpy(&var_data, &node_data_left->value, sizeof(Variable_data*));
    int64_t var_index = Find_variable(&func_list->func_data[index].parameters,
                                      var_data->var_name, var_data->var_len);
    if(var_index == -1) {

        int64_t loc_var_mem_index = func_list->func_data[index].parameters.free_var;
        var_index = Find_variable(&func_list->func_data[index].local_vars,
                                  var_data->var_name, var_data->var_len);
        if(var_index == -1) {

            DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
            return false;
        }

        PRINTOUT("POP [BX+%d]\n", var_index + loc_var_mem_index);
    }

    else {

        PRINTOUT("POP [BX+%d]\n", var_index);
    }

    return true;
}

bool Compile_operator(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));

    if(tmp_data->expression_type == OP) {

        if(root->left_node)
            if(!Compile_operator(output_file, root->left_node, func_list, index))
                return false;

        if(root->right_node)
            if(!Compile_operator(output_file, root->right_node, func_list, index))
                return false;

        switch(tmp_data->value) {

            #define FUNC(value, symbol) case value:\
                                            PRINTOUT(#value "\n");\
                                            break;

            #define FUNC_BASIC(value, symbol) case value:\
                                            PRINTOUT(#value "\n");\
                                            break;

            #include "Funcs_code_gen.h"
            #undef FUNC
            #undef FUNC_BASIC

            default:
                DEBUG_PRINTF("ERROR: no such function\n");
                return false;
        }
    }

    else if(tmp_data->expression_type == VAR) {

        Variable_data* var_data = NULL;
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
        int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                        var_data->var_name, var_data->var_len);
        if(var_index == -1) {

            var_index = Find_variable(&func_list->func_data[index].parameters,
                                    var_data->var_name, var_data->var_len);
            if(var_index == -1) {

                DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
                return false;
            }

            PRINTOUT("PUSH [BX+%d]\n", var_index);
        }

        else {

            int64_t loc_var_mem_index = func_list->func_data[index].parameters.free_var;
            PRINTOUT("PUSH [BX+%d]\n", var_index + loc_var_mem_index);
        }
    }

    else if(tmp_data->expression_type == NUM) {

        double tmp_double = 0;
        memcpy(&tmp_double, &tmp_data->value, sizeof(double));
        PRINTOUT("PUSH %lg\n", tmp_double);
    }

    else if(tmp_data->expression_type == FUNCTION) {

        if(!Compile_user_function(output_file, root, func_list, index))
            return false;
    }

    else {

        DEBUG_PRINTF("ERROR: unknown operator\n");
        return false;
    }

    return true;
}

bool Compile_user_function(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    Variable_data* var_data = NULL;
    memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));

    if(!strncmp(var_data->var_name, "Out", var_data->var_len)) {

        if(!Compile_push_parameters(output_file, root->left_node, func_list, index))
            return false;

        PRINTOUT("OUT\n");
    }

    else {

        int64_t func_memory = func_list->func_data[index].func_memory;
        PRINTOUT("PUSH BX\n");

        if(!Compile_push_parameters(output_file, root->left_node, func_list, index))
            return false;

        PRINTOUT("PUSH BX\n");
        PRINTOUT("PUSH %d\n", func_memory);
        PRINTOUT("ADD\n");
        PRINTOUT("POP BX\n");
        PRINTOUT("CALL %.*s\n", (size_t) var_data->var_len, var_data->var_name);
    }

    return true;
}

bool Compile_push_parameters(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == COMMA && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node)
            if(!Compile_push_parameters(output_file, root->left_node, func_list, index))
                return false;
        if(root->right_node)
            if(!Compile_push_parameters(output_file, root->right_node, func_list, index))
                return false;
    }

    else {

        if(!Compile_operator(output_file, root, func_list, index))
            return false;
    }

    return true;
}
