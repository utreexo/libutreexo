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
#include "mmap_forest.h"
#include "parent_hash.h"
#include "util.h"

static const char UTREEXO_ZERO_HASH[32] = {0};
static inline void utreexo_forest_add(struct utreexo_forest *p,
                                      utreexo_node_hash leaf) {
  utreexo_forest_node node = {
      .hash = {0}, .parent = NULL, .left_child = NULL, .right_child = NULL};
  memcpy(node.hash.hash, leaf.hash, 32);
  const uint64_t nLeaves = p->nLeaf;
  uint8_t height = 0;
  utreexo_forest_node *pnode = NULL;
  utreexo_forest_file_node_put(p->data, &pnode, node);

  while ((nLeaves >> height & 1) == 1) {
    utreexo_forest_node *root = p->roots[height];
    DEBUG_ASSERT(root != NULL);
    p->roots[height] = NULL;
    if (memcmp(root->hash.hash, UTREEXO_ZERO_HASH, 32) == 0) {
      break;
    }

    utreexo_forest_node new_root = {
        .parent = NULL, .left_child = root, .right_child = pnode};

    parent_hash(new_root.hash.hash, root->hash.hash, pnode->hash.hash);

    utreexo_forest_node *new_root_pos = NULL;
    utreexo_forest_file_node_put(p->data, &new_root_pos, new_root);

    pnode->parent = new_root_pos;
    root->parent = new_root_pos;

    pnode = new_root_pos;
    height++;
  }
  DEBUG_ASSERT(p->roots[height] == NULL);
  p->roots[height] = pnode;
  ++p->nLeaf;
}

#endif
