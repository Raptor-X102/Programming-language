#ifndef BIN_TREE_GRAPH_DEBUG
#define BIN_TREE_GRAPH_DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "Tree_definitions.h"
#include "DEBUG_PRINTF.h"
#include "..\headers\Lang_definitions.h"

#define PRINTOUT(...) fprintf(output_file, __VA_ARGS__)

const int Calloc_add = 10; // in case user inserts filename with 1 char extension. PNG extension requires 3 chars. Additionally 4 byte for SPACES, -o.

void Tree_graph_debug(Node* root, const char* output_file_name, void (*Print_tree_data) (const void* data, FILE* file));
void Create_nodes(Node* root, FILE* output_file, void (*Print_tree_data) (const void* data, FILE* file));
void Create_nodes_akinator(Node* root, FILE* output_file, void (*Print_tree_data) (const void* data, FILE* file));
void Create_edges(Node* root, FILE* output_file);
void Create_edges_akinator(Node* root, FILE* output_file);
bool Check_image_existence(const void* filename_ptr);
bool System_call_GraphViz(const char* output_file_name);

#endif
