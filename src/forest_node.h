#ifndef UTREEXO_FOREST_NODE_H
#define UTREEXO_FOREST_NODE_H
#include "parent_hash.h"

/* A node inside our forest, may be either a branch or a leaf, holds a hash and
 * a few pointers to: (i) parent (ii) left child (if not leaf) (ii) right child
 * (if not leaf) */
typedef struct utreexo_forest_node {
  utreexo_node_hash hash;
  struct utreexo_forest_node *parent;
  struct utreexo_forest_node *left_child;
  struct utreexo_forest_node *right_child;
} __attribute__((__packed__)) utreexo_forest_node;

#endif
