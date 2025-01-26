#ifndef TREE_DEF
#define TREE_DEF

#include <stdint.h>

struct Node {

    int64_t node_data; // node data is 8 bytes
    Node* left_node;
    Node* right_node;
};

#endif
