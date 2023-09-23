#ifndef UTREEXO_FOREST_NODE_H
#define UTREEXO_FOREST_NODE_H
#include "parent_hash.h"

typedef struct {
  uint64_t offset;
  uint64_t page;
} utreexo_forest_node_ptr;

typedef struct {
  utreexo_node_hash hash;
  utreexo_forest_node_ptr parent;
  utreexo_forest_node_ptr left_child;
  utreexo_forest_node_ptr right_child;
} utreexo_forest_node;

#endif