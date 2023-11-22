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
  utreexo_forest_node *pnode = utreexo_forest_file_node_alloc(p->data);
  *pnode = (utreexo_forest_node){
      .hash = {{0}}, .parent = NULL, .left_child = NULL, .right_child = NULL};
  memcpy(pnode->hash.hash, leaf.hash, 32);

  const uint64_t nLeaves = p->nLeaf;
  uint8_t height = 0;

  while ((nLeaves >> height & 1) == 1) {
    utreexo_forest_node *root = p->roots[height];
    debug_assert(root != NULL);
    p->roots[height] = NULL;
    if (memcmp(root->hash.hash, UTREEXO_ZERO_HASH, 32) == 0) {
      break;
    }

    utreexo_forest_node *proot = utreexo_forest_file_node_alloc(p->data);
    *proot = (utreexo_forest_node){
        .parent = NULL, .left_child = root, .right_child = pnode};

    parent_hash(proot->hash.hash, root->hash.hash, pnode->hash.hash);

    pnode->parent = proot;
    root->parent = proot;

    pnode = proot;
    height++;
  }
  debug_assert(p->roots[height] == NULL);
  p->roots[height] = pnode;
  ++p->nLeaf;
}

static inline void grab_node(struct utreexo_forest *f,
                             utreexo_forest_node **node,
                             utreexo_forest_node **sibling,
                             utreexo_forest_node **parent, uint64_t pos) {
  node_offset offset = detect_offset(pos, f->nLeaf);

  debug_assert(offset.tree < 64);

  utreexo_forest_node *pnode = f->roots[offset.tree];
  utreexo_forest_node *psibling = NULL;
  utreexo_forest_node *pparent = NULL;
  if (offset.depth == 0) {
    *node = pnode;
    *sibling = psibling;
    *parent = pparent;

    return;
  }
  for (size_t h = offset.depth; h > 0; --h) {
    uint32_t mask = 1 << (h - 1);

    pparent = pnode;

    if (pnode->right_child == NULL) {
      pnode = NULL;
      break;
    }
    if (mask & pos) {
      pnode = pparent->right_child;
      psibling = pparent->left_child;
    } else {
      psibling = pparent->right_child;
      pnode = pparent->left_child;
    }
  }

  *node = pnode;
  *sibling = psibling;
  *parent = pparent;
}

static inline void utreexo_forest_free(struct utreexo_forest *forest) {
  utreexo_forest_file_close(forest->data);
  free(forest);
}

static inline void recompute_parent_hash(utreexo_forest_node *origin) {
  utreexo_forest_node *pnode = origin->parent;
  while (pnode != NULL) {
    parent_hash(pnode->hash.hash, pnode->left_child->hash.hash,
                pnode->right_child->hash.hash);
    pnode = pnode->parent;
  }
}

static inline int delete_single(struct utreexo_forest *f, uint64_t pos) {
  utreexo_forest_node *pnode, *psibling, *pparent;

  node_offset offset = detect_offset(pos, f->nLeaf);
  grab_node(f, &pnode, &psibling, &pparent, pos);

  if (!pnode)
    return -1;

  if (pparent == NULL) {
    f->roots[offset.tree] = NULL;
    return 0;
  }

  if (pparent->parent != NULL) {
    if (pparent->parent->right_child == pparent)
      pparent->parent->right_child = psibling;
    else
      pparent->parent->left_child = psibling;
  } else {
    f->roots[offset.tree] = psibling;
  }
  recompute_parent_hash(psibling);
  return 0;
}

#endif
