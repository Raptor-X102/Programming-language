#ifndef TREE_FUNCS
#define TREE_FUNCS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Tree_definitions.h"
#include "DEBUG_PRINTF.h"
#include "GetFileSize2.h"
#include "Stack_Functions.h"

#define PRINTOUT(...) fprintf(output_file, __VA_ARGS__)

bool Create_new_node(Node** node, const void* data, size_t data_size);
void Insert_node(Node* node, const void* data, size_t data_size,
                 Node** (*Compare_func) (Node* root, const void* search_data,
                                         size_t search_data_size));
void Replace_node(Node** initial_node, const void* replacing_value, const void* new_node_value,
                  size_t values_size, Node** (*Compare_func) (Node* root,
                                                              const void* search_data,
                                                              size_t search_data_size));
void Replace_node_auto(Node** initial_node, const void* searching_value,
                       const void* replacing_value, const void* new_node_value,
                       size_t values_size, Node** (*Compare_func) (Node* root,
                                                                   const void* search_data,
                                                                   size_t search_data_size));
void Output_tree(const Node* root, size_t values_size, const char* filename,
                 void (*Print_strings)(const void* data, FILE* file));
void Get_tree_data(const Node* root, Stack_t* stack);
void Print_tree_data(Stack_t* stack, FILE* output_file, uint64_t offset,
                     void (*Print_strings)(const void* data, FILE* file));
void Make_offset(FILE* output_file, uint64_t offset);
void Output_node_definition(const Node* root, const char* searching_value);
bool Find_node_definition(const Node* root, const char* searching_value, Stack_t* stack);
void Print_node_definition(Stack_t* stack);
char* Read_tree_file(Node** root, const char* filename);
void Create_tree(Node** node, char* node_data, uint64_t data_size, uint64_t* curr_pos);
void Get_next_symbol(const char* node_data, uint64_t data_size, uint64_t* curr_pos);
uint64_t Get_word(char* node_data, uint64_t data_size, uint64_t* curr_pos);
void Tree_Dtor(Node* root);

#endif
