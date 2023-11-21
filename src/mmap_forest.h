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
  utreexo_forest_node *roots[64];
  uint64_t nLeaf;
};

/* Adds one node to the forest. */
static inline void utreexo_forest_add(struct utreexo_forest *p,
                                      utreexo_node_hash leaf);
/* Frees a forest. */
static inline void utreexo_forest_free(struct utreexo_forest *p);

#endif // MMAP_FOREST_H
