#include "Lang_funcs.h"

Node* Lang_new_node(int node_type, const void* node_value, size_t node_val_size,
                    Node* left_node, Node* right_node) {

    Node_data* node_data_ptr = (Node_data*) calloc(Mem_blocks_amount, sizeof(Node_data));
    if(!node_data_ptr) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    node_data_ptr->expression_type = node_type;
    memcpy(&node_data_ptr->value, node_value, node_val_size);

    Node* node = NULL;
    if(!Create_new_node(&node, &node_data_ptr, sizeof(Node_data*)))
        return NULL;

    node->left_node = left_node;
    node->right_node = right_node;

    return node;
}

void Print_node_data(const void* data, FILE* file) {

    Node_data* tmp_node_data = NULL;
    memcpy(&tmp_node_data, data, sizeof(Node_data*));
    switch(tmp_node_data->expression_type) {
        case NUM: {

            fprintf(file, "expression_type = %#X (NUM), \\n ", NUM);
            double tmp_double = 0;
            memcpy(&tmp_double, &tmp_node_data->value, sizeof(double));

            if(fabs(tmp_double - M_PI) < Epsilon)
                fprintf(file, "value = pi");

            else if(fabs(tmp_double - M_E) < Epsilon)
                fprintf(file, "value = e");

            else
                fprintf(file, "value = %.4lg", tmp_double);
            break;
        }

        case VAR: {

            fprintf(file, "expression_type = %#X (VAR), \\n ", VAR);
            Variable_data* tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(Variable_data*));
            fprintf(file, "value = %.*s", (size_t) tmp_int->var_len, tmp_int->var_name);
            break;
        }

        case OP: {

            fprintf(file, "expression_type = %#X (OP), \\n ", OP);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define FUNC(func_name, func_full_name) case func_name:\
                                                               fprintf(file, "value = %s ", func_full_name);\
                                                               fprintf(file, "%#X", func_name);\
                                                               break;
                #define FUNC_BASIC(func_name, func_full_name) case func_name:\
                                                               fprintf(file, "value = %s ", func_full_name);\
                                                               fprintf(file, "%#X", func_name);\
                                                               break;
                #include "Funcs_code_gen.h"
                #undef FUNC
                #undef FUNC_BASIC

                default:

                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }
            fprintf(file, "\n");
            if(!tmp_int)
                DEBUG_PRINTF("ERROR: wrong expression type (0 operation)\n");

            break;
        }

        case FUNCTION: {

            fprintf(file, "expression_type = %#X (FUNCTION), \\n ", FUNCTION);
            Variable_data* tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(Variable_data*));
            fprintf(file, "value = %.*s", (size_t) tmp_int->var_len, tmp_int->var_name);
            break;
        }

        case CONDITION: {

            fprintf(file, "expression_type = %#X (CONDITION), \\n ", CONDITION);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define CONDITION(value, symbol, spu_code) case value:\
                                                    fprintf(file, "value = ");\
                                                    fprintf(file, "%#X", value);\
                                                    break;

                #include "Conditions_code_gen.h"
                #undef FUNC

                default:
                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }

            break;
        }

        case SPECIAL_SYMBOL: {

            fprintf(file, "expression_type = %#X (SPECIAL_SYMBOL), \\n ", SPECIAL_SYMBOL);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define SPECIAL_SYMBOL(value, symbol) case value:\
                                                          fprintf(file, "value = ");\
                                                          fprintf(file, "%#X", value);\
                                                          break;

                #include "Special_symbols_code_gen.h"
                #undef FUNC

                default:
                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }

            break;
        }

        case KEY_WORD: {

            fprintf(file, "expression_type = %#X (KEY_WORD), \\n ", KEY_WORD);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define KEY_WORD(value, symbol) case value:\
                                                    fprintf(file, "value = %s ", symbol);\
                                                    fprintf(file, "%#X", value);\
                                                    break;

                #include "Key_words_code_gen.h"
                #undef FUNC

                default:
                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }

            break;
        }

        case IF_BRANCHES: {

            fprintf(file, "expression_type = %#X (IF_BRANCHES), \\n ", IF_BRANCHES);
            fprintf(file, "value = CONNECTOR");
            break;
        }

        default:
            DEBUG_PRINTF("ERROR: wrong expression type\n %#X", tmp_node_data->expression_type);
    }
}

void Lang_dtor(Node* root) {

    if(root) {

        if(root->left_node)
            Lang_dtor(root->left_node);

        if(root->right_node)
            Lang_dtor(root->right_node);

        Node_data* tmp_node_data_ptr = NULL;
        memcpy(&tmp_node_data_ptr, &root->node_data, sizeof(Node_data*));

        free(tmp_node_data_ptr);
        free(root);
    }
}
