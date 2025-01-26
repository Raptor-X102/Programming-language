#include <TXLib.h>
#include "Lang_funcs.h"
#include "..\Check_flags\Check_r_w_flags.h"
#include "..\Binary_Trees\Binary_tree_graph_debug.h"

int main(int argc, char* argv[]) {

    char* read_files[1] = {};
    char* write_files[3] = {};
    file_types files = {};
    files.read_files = read_files;
    files.write_files = write_files;
    if(!Check_r_w_flags(CHECK_R_W, argv, argc, &files)) {

        DEBUG_PRINTF("ERROR: flags verification failed")
        return 0;
    }

    Buffer_data expression_buffer = {};

    Var_list var_list_variables = {};
    Var_list var_list_functions = {};

    if(!Var_list_ctor(&var_list_variables, Var_list_init_size)) {

        DEBUG_PRINTF("ERROR: var_list null pointer\n");
        return false;
    }

    if(!Var_list_ctor(&var_list_functions, Var_list_init_size)) {

        DEBUG_PRINTF("ERROR: var_list null pointer\n");
        return false;
    }
    Func_data_list func_list = {};
    Lexeme_array* lexeme_arr = NULL;
    Node* root = Lang_read_expression(files.read_files[0], &var_list_variables,
                                      &var_list_functions, &expression_buffer, &func_list,
                                      &lexeme_arr);

    if(!root) {

        DEBUG_PRINTF("ERROR: function returned NULL\n");
        return false;
    }

    DEBUG_PRINTF("VARIABLES\n");
    for(int i = 0; i < var_list_variables.free_var; i++) {

        DEBUG_PRINTF("var_len = %d ", var_list_variables.var_list[i].var_len);
        DEBUG_PRINTF("var_name = %p ", var_list_variables.var_list[i].var_name);
        DEBUG_PRINTF("%.*s ", (size_t) var_list_variables.var_list[i].var_len,
                                       var_list_variables.var_list[i].var_name);
         DEBUG_PRINTF("\n");
    }
    DEBUG_PRINTF("\nFUNCTIONS\n");
    for(int i = 0; i < var_list_variables.free_var; i++) {

        DEBUG_PRINTF("var_len = %d ", var_list_functions.var_list[i].var_len);
        DEBUG_PRINTF("var_name = %p ", var_list_functions.var_list[i].var_name);
        DEBUG_PRINTF("%.*s ", (size_t) var_list_functions.var_list[i].var_len,
                                       var_list_functions.var_list[i].var_name);
         DEBUG_PRINTF("\n");
    }
    DEBUG_PRINTF("\nFUNCTIONS IN FUNC_LIST\n");
    for(int i = 0; i < func_list.free_element; i++) {

        DEBUG_PRINTF("\nvar_len = %d ", func_list.func_data[i].function->var_len);
        DEBUG_PRINTF("var_name = %p ", func_list.func_data[i].function->var_name);
        DEBUG_PRINTF("%d ", i);
        DEBUG_PRINTF("%.*s ", (size_t) func_list.func_data[i].function->var_len,
                                       func_list.func_data[i].function->var_name);
        DEBUG_PRINTF("%d\n", func_list.func_data[i].func_memory);
        for(int j = 0; j < func_list.func_data[i].parameters.free_var; j++) {

            DEBUG_PRINTF("%d. ", j);
            DEBUG_PRINTF("var_len = %d ", (size_t) func_list.func_data[i].parameters.var_list[j].var_len);
            DEBUG_PRINTF("var_name = %p ", func_list.func_data[i].parameters.var_list[j].var_name);
            DEBUG_PRINTF("%.*s, ", (size_t) func_list.func_data[i].parameters.var_list[j].var_len,
                                            func_list.func_data[i].parameters.var_list[j].var_name);
            DEBUG_PRINTF("\n");
        }
        DEBUG_PRINTF("\n");
        for(int j = 0; j < func_list.func_data[i].local_vars.free_var; j++) {

            DEBUG_PRINTF("%d. ", j);
            DEBUG_PRINTF("var_len = %d ", (size_t) func_list.func_data[i].local_vars.var_list[j].var_len);
            DEBUG_PRINTF("var_name = %p ", func_list.func_data[i].local_vars.var_list[j].var_name);
            DEBUG_PRINTF("%.*s, ", (size_t) func_list.func_data[i].local_vars.var_list[j].var_len,
                                            func_list.func_data[i].local_vars.var_list[j].var_name);
            DEBUG_PRINTF("\n");
        }
        DEBUG_PRINTF("\n");
    }
    //getchar();
    Tree_graph_debug(root, files.write_files[0], &Print_node_data);
    if(!Compile_to_ASM(files.write_files[1], root, &func_list))
        return 0;
    DEBUG_PRINTF("RETURNED TO MAIN\n");
    DEBUG_PRINTF("root = %p\n", root);
    DEBUG_PRINTF("TREE DONE\n");

    Labels labels = {};
    Labels_Ctor(&labels, Labels_amount);

    //DEBUG_PRINTF("start\n");

    Compile_code(files.write_files[1], files.write_files[2], &labels);

    //DEBUG_PRINTF("end\n");

    SPU_data processor = {};

    SPU_Ctor(&processor, files.write_files[2]);
    //DEBUG_PRINTF("\n1\n");

    //DEBUG_PRINTF("&processor = %p", &processor);
    SPU_Run(&processor);
    //DEBUG_PRINTF("\n2\n");

    SPU_Dump(&processor, Printf_Format_Function);

    //DEBUG_PRINTF("\n3\n");
    Labels_Dtor(&labels);
    SPU_Dtor(&processor);
    free(lexeme_arr->lex_arr);
    free(lexeme_arr);
    for(int i = 0; i < func_list.free_element; i++) {

        Var_list_dtor(&func_list.func_data[i].local_vars);
        Var_list_dtor(&func_list.func_data[i].parameters);
    }
    DEBUG_PRINTF("TREE DONE\n");
    //free(func_list.func_data);
    free(expression_buffer.buffer);
    Var_list_dtor(&var_list_variables);
    Var_list_dtor(&var_list_functions);
}
