#include "Tree_funcs.h"

void Printf_Format_Func(const void * value);

bool Create_new_node(Node** node, const void* data, size_t data_size) {

    Node* tmp_ptr = (Node*) calloc(1, sizeof(Node));
    if(tmp_ptr) {

        *node = tmp_ptr;
        tmp_ptr = NULL;
    }

    else {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return false;
    }

    DEBUG_PRINTF("(*node)->node_data = %p\n", (*node)->node_data);
    DEBUG_PRINTF("data = %p\n", data);
    memcpy(&(*node)->node_data, data, data_size);

    return true;
}

void Insert_node(Node* node, const void* data, size_t data_size,
                 Node** (*Compare_func) (Node* root, const void* search_data,
                                         size_t search_data_size)) {

    Node** tmp_node_ptr = Compare_func(node, data, data_size);

    if(!(*tmp_node_ptr))
        Create_new_node(tmp_node_ptr, data, data_size);

    else
        Insert_node(*tmp_node_ptr, data, data_size, Compare_func);
}

void Replace_node(Node** initial_node, const void* replacing_value, const void* new_node_value,
                  size_t values_size, Node** (*Compare_func) (Node* root,
                                                              const void* search_data,
                                                              size_t search_data_size)) {

    if((*initial_node)->left_node == NULL && (*initial_node)->right_node == NULL){

        Node* initial_node_copy = *initial_node;

        Create_new_node(initial_node, replacing_value, values_size);

        Node** replacing_node = Compare_func(*initial_node,
                                             &initial_node_copy->node_data,
                                             values_size);

        *replacing_node = initial_node_copy;

        if(!(*initial_node)->left_node)
            Create_new_node(&((*initial_node)->left_node), new_node_value, values_size);

        else
            Create_new_node(&((*initial_node)->right_node), new_node_value, values_size);

        return;
    }

    Node** tmp_node_ptr = Compare_func(*initial_node, new_node_value, values_size);

    Replace_node(tmp_node_ptr, replacing_value, new_node_value, values_size, Compare_func);
}

void Replace_node_auto(Node** initial_node, const void* searching_value,
                       const void* replacing_value, const void* new_node_value,
                       size_t values_size, Node** (*Compare_func) (Node* root,
                                                                   const void* search_data,
                                                                   size_t search_data_size)) {

    if((*initial_node)->left_node == NULL && (*initial_node)->right_node == NULL) {

        char* tmp_char_ptr = NULL, * sv_ptr = NULL;
        memcpy(&tmp_char_ptr, &(*initial_node)->node_data, values_size);
        memcpy(&sv_ptr, searching_value, values_size);

        if(!strcmp(tmp_char_ptr, sv_ptr)) {

            Node* initial_node_copy = *initial_node;

            Create_new_node(initial_node, replacing_value, values_size);

            Node** replacing_node = Compare_func(*initial_node, searching_value, values_size);
            *replacing_node = initial_node_copy;

            if(!(*initial_node)->left_node)
                Create_new_node(&((*initial_node)->left_node), new_node_value, values_size);

            else
                Create_new_node(&((*initial_node)->right_node), new_node_value, values_size);

            return;
        }
    }

    else {

        Replace_node_auto(&(*initial_node)->left_node, searching_value,
                          replacing_value, new_node_value, values_size, Compare_func);

        Replace_node_auto(&(*initial_node)->right_node, searching_value,
                          replacing_value, new_node_value, values_size, Compare_func);
    }
}

void Output_tree(const Node* root, size_t values_size, const char* filename,
                 void (*Print_strings)(const void* data, FILE* file)) {

    FILE* output_file = fopen(filename, "wb+");
    if(!output_file) {

        DEBUG_PRINTF("ERROR: file %s was not opened\n", filename);
        return;
    }

    Stack_t stack = {};
    STACK_CTOR(&stack, 10, values_size);

    Get_tree_data(root, &stack);

    Print_tree_data(&stack, output_file, 0, Print_strings);
}

void Get_tree_data(const Node* root, Stack_t* stack) {

    DEBUG_PRINTF("\nroot = %p\n", root);
    DEBUG_PRINTF("root->left_node = %p\n", root->left_node);
    DEBUG_PRINTF("root->right_node = %p\n", root->right_node);
    char* tmp_char_ptr = NULL;
    memcpy(&tmp_char_ptr, &root->node_data, sizeof(char*));
    DEBUG_PRINTF("root->node_data = %s\n\n", tmp_char_ptr);

    if(root->left_node == NULL && root->right_node == NULL) {

        int64_t is_leaf = 0x1;
        StackPush(stack, &is_leaf);
        StackPush(stack, &root->node_data);
    }

    else{

        if(root->right_node)
            Get_tree_data(root->right_node, stack);

        if(root->left_node)
            Get_tree_data(root->left_node, stack);

        int64_t is_leaf = 0x0;
        StackPush(stack, &is_leaf);
        StackPush(stack, &root->node_data);
    }
}

void Print_tree_data(Stack_t* stack, FILE* output_file, uint64_t offset,
                     void (*Print_strings)(const void* data, FILE* file)) {


    char* tmp_str_ptr = NULL;
    int64_t is_leaf = 0;
    StackPop(stack, &tmp_str_ptr);
    StackPop(stack, &is_leaf);

    if(!is_leaf) {

        Make_offset(output_file, offset);
        PRINTOUT("{\n");
        Make_offset(output_file, offset+1);

        PRINTOUT("\"");
        Print_strings(&tmp_str_ptr, output_file);
        PRINTOUT("\"\n");

        Print_tree_data(stack, output_file, offset+1, Print_strings);
        Print_tree_data(stack, output_file, offset+1, Print_strings);

        Make_offset(output_file, offset);
        PRINTOUT("}\n");
    }

    else {

        Make_offset(output_file, offset);
        PRINTOUT("{");
        PRINTOUT("\"%s\"}\n", tmp_str_ptr);
    }
}

void Make_offset(FILE* output_file, uint64_t offset) {

    for(uint64_t i = 0; i < offset; i ++)
        PRINTOUT("    ");
}

void Tree_Dtor(Node* root) {

    if(root->left_node)
        Tree_Dtor(root->left_node);

    if(root->right_node)
        Tree_Dtor(root->right_node);

    free(root);
}


char* Read_tree_file(Node** root, const char* filename) {

    FILE* input_file = fopen(filename, "rb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file %s was not opened\n", filename);
        return NULL;
    }

    int64_t file_size = get_file_size(filename);
    DEBUG_PRINTF("file_size = %d\n", file_size);
    char* nodes_data = (char*) calloc(file_size, sizeof(char));
    if(!nodes_data) {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    fread(nodes_data, sizeof(char), file_size, input_file);
    if(ferror(input_file)) {

        DEBUG_PRINTF("ERROR: fread failed\n");
        return NULL;
    }

    uint64_t curr_pos = 0;
    Create_tree(root, nodes_data, file_size, &curr_pos);

    return nodes_data;
}

void Create_tree(Node** node, char* node_data, uint64_t data_size, uint64_t* curr_pos) {

    DEBUG_PRINTF("(start) curr_pos = %d\n", *curr_pos);
    Get_next_symbol(node_data, data_size, curr_pos);

    if(node_data[*curr_pos] == '{') {

        (*curr_pos)++;
        Get_next_symbol(node_data, data_size, curr_pos);
        uint64_t word_end_pos = Get_word(node_data, data_size, curr_pos);

        char* string_ptr = node_data+ *curr_pos + 1;
        DEBUG_PRINTF("string_ptr = %p\n", string_ptr);
        DEBUG_PRINTF("string_ptr = %c\n", *string_ptr);
        DEBUG_PRINTF("string_ptr = %s\n", string_ptr);

        Create_new_node(node, &string_ptr, sizeof(char*));

        DEBUG_PRINTF("node = %p\n", *node);

        char* tmp_char_ptr = NULL;
        memcpy(&tmp_char_ptr, &(*node)->node_data, sizeof(char*));
        DEBUG_PRINTF("(*node)->node_data = %p\n", tmp_char_ptr);
        DEBUG_PRINTF("(*node)->node_data = %s\n", tmp_char_ptr);

        *curr_pos = word_end_pos;
        Get_next_symbol(node_data, data_size, curr_pos);
        DEBUG_PRINTF("(before recurse) curr_pos = %d\n", *curr_pos);
        if(node_data[*curr_pos] == '}') {

            (*curr_pos)++;
            DEBUG_PRINTF("(end) curr_pos = %d\n", *curr_pos);
            return;
        }

        else {

            if((*node)->left_node) {

                Create_tree(&(*node)->left_node, node_data, data_size, curr_pos);
                DEBUG_PRINTF("(after 1) curr_pos = %d\n", *curr_pos);
            }

            if((*node)->right_node) {

                Create_tree(&(*node)->right_node, node_data, data_size, curr_pos);
                DEBUG_PRINTF("(after 2) curr_pos = %d\n", *curr_pos);
            }

            Get_next_symbol(node_data, data_size, curr_pos);
            (*curr_pos)++;
        }
        DEBUG_PRINTF("(end) curr_pos = %d\n", *curr_pos);
    }
}

void Get_next_symbol(const char* node_data, uint64_t data_size, uint64_t* curr_pos) {

    while(*curr_pos < data_size) {
        while(isspace(node_data[*curr_pos]))
            (*curr_pos)++;

        if(node_data[*curr_pos] == ';') {

            while(node_data[*curr_pos] != '\n')
                (*curr_pos)++;

            (*curr_pos)++;
            continue;
        }

        return;
    }
}

uint64_t Get_word(char* node_data, uint64_t data_size, uint64_t* curr_pos) {

    uint64_t word_end_pos = *curr_pos + 1;

        while(*curr_pos < data_size && node_data[word_end_pos] != '\"')
            (word_end_pos)++;

        node_data[word_end_pos] = '\0';

        return word_end_pos+1;
}

void Output_node_definition(const Node* root, const char* searching_value) {

    Stack_t stack = {};
    STACK_CTOR(&stack, 10, sizeof(char*));
    Find_node_definition(root, searching_value, &stack);

    printf("%s:", searching_value);
    Print_node_definition(&stack);
}

bool Find_node_definition(const Node* root, const char* searching_value, Stack_t* stack) {

    if(root->left_node == NULL && root->right_node == NULL) {

        char* tmp_char_ptr = NULL;
        memcpy(&tmp_char_ptr, &root->node_data, sizeof(char*));
        if(!strcmp(tmp_char_ptr, searching_value))
            return true;

        else
            return false;
    }

    else {

        if(root->left_node) {

            if(Find_node_definition(root->left_node, searching_value, stack)) {

                StackPush(stack, &root->node_data);
                int64_t property_status = 0x0;
                StackPush(stack, &property_status);
                return true;
            }
        }

        if(root->right_node) {

            if(Find_node_definition(root->right_node, searching_value, stack)) {

                StackPush(stack, &root->node_data);
                int64_t property_status = 0x1;
                StackPush(stack, &property_status);
                return true;
            }
        }

        return false;
    }
}

void Print_node_definition(Stack_t* stack) {

    if(!stack->size) {

        printf("No such value in tree\n");
        return;
    }

    while(stack->size) {

        char* property_status = NULL, * property = NULL;
        StackPop(stack, &property_status);
        StackPop(stack, &property);

        if(property_status)
            printf("     %.*s", strlen(property)-1, property);

        else
            printf(" not %.*s", strlen(property)-1, property);

        if(stack->size)
            printf(",");

        else
            printf(".");
    }

    printf("\n");
}

void Printf_Format_Func(const void * value) {

    char* tmp_char_ptr = NULL;
    memcpy(&tmp_char_ptr, value, sizeof(char*));
    DEBUG_PRINTF("%p", tmp_char_ptr);
}
