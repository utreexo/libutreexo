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

static inline void grab_node(struct utreexo_forest *f,
                             utreexo_forest_node **node,
                             utreexo_forest_node **sibling,
                             utreexo_forest_node **parent, uint64_t pos) {
  node_offset offset = detect_offset(pos, f->nLeaf);

  DEBUG_ASSERT(offset.tree < 64);

  utreexo_forest_node *pnode = f->roots[offset.tree];
  utreexo_forest_node *psibling = NULL;
  utreexo_forest_node *pparent = NULL;

  for (size_t h = 0; h < offset.depth; ++h) {
    if (pnode->right_child == NULL)
      break;

    pparent = pnode;

    if ((offset.bits >> h & 1) == 1) {
      pnode = pparent->right_child;
      psibling = pparent->left_child;
    } else {
      psibling = pparent->right_child;
      pnode = pparent->right_child;
    }
  }

  *node = pnode;
  *sibling = psibling;
  *parent = pparent;
}

void utreexo_forest_print(utreexo_forest_node *f) {
  utreexo_forest_node **nodes;
  *nodes = (utreexo_forest_node *)malloc(1024);
  nodes[0] = f;

  utreexo_forest_node *next[1000] = {0};
  size_t i = 0;
  do {
    utreexo_forest_node *pnode = nodes[i];
    if (pnode == NULL) {
      printf("=====", pnode->hash.hash[0], pnode->hash.hash[1]);
      next[i * 2] = NULL;
      next[i * 2 + 1] = NULL;
    }
    printf("%02x%02x", pnode->hash.hash[0], pnode->hash.hash[1]);

    next[i * 2] = pnode->left_child;
    next[i * 2 + 1] = pnode->right_child;
  } while (21);
}
static inline void delete_single(struct utreexo_forest *f, uint64_t pos) {
  utreexo_forest_node *pnode, *psibling, *pparent;

  node_offset offset = detect_offset(pos, f->nLeaf);
  grab_node(f, &pnode, &psibling, &pparent, pos);

  if (pparent == NULL)
    f->roots[offset.tree] = NULL;

  if (pparent->parent != NULL) {
    if (pparent->parent->right_child == pparent)
      pparent->parent->right_child = psibling;
    else
      pparent->parent->left_child = psibling;
    return;
  }
  f->roots[offset.tree] = psibling;
}

#endif
