#include "Backend_funcs.h"

bool Compile_to_ASM(const char* out_file, Node* root, Func_data_list* func_list) {

    FILE* output_file = fopen(out_file, "wb+");
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

    Compile_user_function_def(output_file, root, func_list);
    fclose(output_file);
    return true;
}

void Compile_user_function_def(FILE* output_file, Node* root, Func_data_list* func_list) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == ANGLE_BRACKET_CL && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node)
            Compile_user_function_def(output_file, root->left_node, func_list);

        if(root->left_node)
            Compile_user_function_def(output_file, root->right_node, func_list);
    }

    else {

        int64_t index = Find_func_data(func_list, &tmp_data->value);
        if(index == -1) {

            DEBUG_PRINTF("ERROR: this function was not inserted in func_data");
            abort();
        }

        PRINTOUT("%.*s:\n", (size_t) func_list->func_data[index].function->var_len,
                                     func_list->func_data[index].function->var_name);
        if(func_list->func_data[index].parameters.free_var > 0)
            for(int i = func_list->func_data[index].parameters.free_var - 1; i >= 0; i--)
                PRINTOUT("POP [BX+%d]\n", i);

        int64_t if_count = 0, while_count = 0;
        Compile_operation(output_file, root->right_node, func_list, index, &if_count, &while_count);
    }
}

void Compile_operation(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                       int64_t* if_count, int64_t* while_count) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == TAB && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node)
            Compile_operation(output_file, root->left_node, func_list, index, if_count, while_count);

        if(root->left_node)
            Compile_operation(output_file, root->right_node, func_list, index, if_count, while_count);
    }

    else {

        if(tmp_data->value == IF && tmp_data->expression_type == KEY_WORD) {

            Compile_condition(output_file, root->left_node, func_list, index);
            Compile_if(output_file, root->right_node, func_list, index, if_count, while_count);
        }

        else if(tmp_data->value == WHILE && tmp_data->expression_type == KEY_WORD)
            Compile_while(output_file, root, func_list, index, if_count, while_count);

        else if(tmp_data->value == ASSIGNMENT && tmp_data->expression_type == SPECIAL_SYMBOL)
            Compile_assignment(output_file, root, func_list, index);

        else if(tmp_data->expression_type == FUNCTION)
            Compile_user_function(output_file, root, func_list, index);

        else if(tmp_data->value == RETURN && tmp_data->expression_type == KEY_WORD)
            Compile_return(output_file, root, func_list, index);
    }

}

void Compile_while(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                   int64_t* if_count, int64_t* while_count) {

    int64_t var_len = func_list->func_data[index].function->var_len;
    char* var_name = func_list->func_data[index].function->var_name;
    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_check_cond:\n", *while_count);
    Compile_condition(output_file, root->left_node, func_list, index);
    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes\n", *while_count);
    PRINTOUT("JMP %.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_no\n\n", *while_count);

    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes:\n", *while_count);
    if(root->right_node)
        Compile_operation(output_file, root->right_node, func_list, index, if_count, while_count);

    PRINTOUT("JMP %.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_check_cond\n", *while_count);
    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_no:\n\n", *while_count);

    (*while_count)++;
}

void Compile_if(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index,
                int64_t* if_count, int64_t* while_count) {

    int64_t var_len = func_list->func_data[index].function->var_len;
    char* var_name = func_list->func_data[index].function->var_name;
    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes\n", *if_count);
    PRINTOUT("JMP %.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_no\n\n", *if_count);

    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes:\n", *if_count);
    if(root->left_node)
        Compile_operation(output_file, root->left_node, func_list, index, if_count, while_count);

    PRINTOUT("JMP %.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes_ret\n", *if_count);
    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_no:\n", *if_count);
    if(root->right_node)
        Compile_operation(output_file, root->right_node, func_list, index, if_count, while_count);

    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes_ret:\n", *if_count);

    (*if_count)++;
}

void Compile_return(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    if(root->right_node)
        Compile_operator(output_file, root->right_node, func_list, index);

    PRINTOUT("POP RX\n");
    PRINTOUT("POP BX\n");
    PRINTOUT("PUSH RX\n");
    PRINTOUT("RET\n");
}

void Compile_condition(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Compile_operator(output_file, root->left_node, func_list, index);
    Compile_operator(output_file, root->right_node, func_list, index);
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
    }

}

void Compile_assignment(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* node_data_left = NULL;
    Variable_data* var_data = NULL;

    Compile_operator(output_file, root->right_node, func_list, index);

    memcpy(&node_data_left, &root->left_node->node_data, sizeof(Node_data*));
    memcpy(&var_data, &node_data_left->value, sizeof(Variable_data*));
    int64_t var_index = Find_variable(&func_list->func_data[index].parameters,
                                      var_data->var_name, var_data->var_len);

    if(var_index == -1) {

        int64_t loc_var_mem_index = func_list->func_data[index].parameters.free_var;
        var_index = Find_variable(&func_list->func_data[index].local_vars,
                                  var_data->var_name, var_data->var_len);
        PRINTOUT("POP [BX+%d]\n", var_index + loc_var_mem_index);
    }

    else {

        PRINTOUT("POP [BX+%d]\n", var_index);
    }

}

void Compile_operator(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));

    if(tmp_data->expression_type == OP) {

        if(root->left_node)
            Compile_operator(output_file, root->left_node, func_list, index);
        if(root->right_node)
            Compile_operator(output_file, root->right_node, func_list, index);

        switch(tmp_data->value) {

            #define FUNC(value, symbol) case value:\
                                            PRINTOUT(#value "\n");\
                                            break;

            #include "Funcs_code_gen.h"
            #undef FUNC

            default:
                DEBUG_PRINTF("ERROR: no such function\n");
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

    else if(tmp_data->expression_type == FUNCTION)
        Compile_user_function(output_file, root, func_list, index);

}

void Compile_user_function(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    Variable_data* var_data = NULL;
    memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));

    if(!strncmp(var_data->var_name, "out", var_data->var_len)) {

        Compile_push_parameters(output_file, root->left_node, func_list, index);
        PRINTOUT("OUT\n");
    }

    else {
        int64_t func_memory = func_list->func_data[index].func_memory;

        PRINTOUT("PUSH BX\n");
        Compile_push_parameters(output_file, root->left_node, func_list, index);
        PRINTOUT("PUSH BX\n");
        PRINTOUT("PUSH %d\n", func_memory);
        PRINTOUT("ADD\n");
        PRINTOUT("POP BX\n");
        PRINTOUT("CALL %.*s\n", (size_t) var_data->var_len, var_data->var_name);
    }
}

void Compile_push_parameters(FILE* output_file, Node* root, Func_data_list* func_list, int64_t index) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == COMMA && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node)
            Compile_push_parameters(output_file, root->left_node, func_list, index);

        if(root->left_node)
            Compile_push_parameters(output_file, root->right_node, func_list, index);
    }

    else {

        /*Variable_data* var_data = NULL;
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
        int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                        var_data->var_name, var_data->var_len);

        if(var_index == -1) {

            var_index = Find_variable(&func_list->func_data[index].parameters,
                                    var_data->var_name, var_data->var_len);
            PRINTOUT("PUSH [BX+%d]\n", var_index);
        }

        else {

            int64_t loc_var_mem_index = func_list->func_data->parameters.free_var;
            PRINTOUT("PUSH [BX+%d]\n", var_index + loc_var_mem_index);
        }*/
        Compile_operator(output_file, root, func_list, index);
    }
}
