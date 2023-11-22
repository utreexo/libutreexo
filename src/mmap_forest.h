#ifndef MMAP_FOREST_H
#define MMAP_FOREST_H

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "flat_file_impl.h"
#include "forest_node.h"
#include "parent_hash.h"
#include "util.h"

struct utreexo_forest {
  struct utreexo_forest_file *data;
  utreexo_forest_node **roots;
  uint64_t *nLeaf;
};

/* Adds one node to the forest. */
static inline void utreexo_forest_add(struct utreexo_forest *p,
                                      utreexo_node_hash leaf);
/* Free up a forest. */
static inline void utreexo_forest_free(struct utreexo_forest *p);

/* Deletes a single node from a forest.
 *
 * This function returns an integer representing whether the operations was
 * successful. 0 means success, -1 means this node doesn't exist in the current
 * forest
 */
static inline int delete_single(struct utreexo_forest *f, uint64_t pos);

/* Walks up the tree and recompute the node hashes */
static inline void recompute_parent_hash(utreexo_forest_node *origin);

static inline void grab_node(struct utreexo_forest *f,
                             utreexo_forest_node **node,
                             utreexo_forest_node **sibling,
                             utreexo_forest_node **parent, uint64_t pos);
#endif // MMAP_FOREST_H
