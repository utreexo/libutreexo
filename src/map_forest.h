#include "flat_file.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef UTREEXO_MAP_FOREST
#define UTREEXO_MAP_FOREST

#include "flat_file_impl.h"
#include "forest_node.h"
#include "parent_hash.h"

struct utreexo_forest {
  struct utreexo_forest_file *data;
  utreexo_forest_node_ptr roots[64];
  uint64_t nLeaf;
};
static const char UTREEXO_ZERO_HASH[32] = {0};
static inline void utreexo_forest_add(struct utreexo_forest *p,
                                            utreexo_node_hash leaf) {
  utreexo_forest_node node = {
      .hash = leaf, .left_child = {.offset = 0}, .right_child = {.offset = 0}};
  uint64_t nLeaves = p->nLeaf;
  uint8_t height = 0;
  utreexo_forest_node_ptr node_pos = {.offset = 0};
  utreexo_forest_file_node_put(p->data, &node_pos, node);

  while (nLeaves & 1) {
    utreexo_forest_node_ptr root = p->roots[height];
    utreexo_forest_node *root_node = NULL;
    utreexo_forest_file_node_get(p->data, &root_node, root);
    if (memcmp(root_node->hash.hash, UTREEXO_ZERO_HASH, 32) == 0) {
      break;
    }
    utreexo_forest_node new_root = {
        .left_child = root, .right_child = node_pos, .parent = {.offset = 0}};
    parent_hash(new_root.hash.hash, root_node->hash.hash, node.hash.hash);
    utreexo_forest_node_ptr new_root_pos = {.offset = 0};
    utreexo_forest_file_node_put(p->data, &new_root_pos, new_root);
    node_pos = new_root_pos;
    node = new_root;
    nLeaves >>= 1;
  }

  p->roots[height] = node_pos;
  ++p->nLeaf;

  DEBUG_PRINT("Out %d\n", p->nLeaf);
  if (p->nLeaf == 2) {
    DEBUG_PRINT("root hash: ");
    for (int i = 0; i < 32; ++i) {
      DEBUG_PRINT("%02x", node.hash.hash[i]);
    }
    DEBUG_PRINT("\n");
  }
}

#endif