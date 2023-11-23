static const int utreexo_forest_version_patch = 0;
static const int utreexo_forest_version_minor = 1;
static const int utreexo_forest_version_major = 0;

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "flat_file.h"
#include "map_forest_impl.h"
#include "mmap_forest.h"
#include "util.h"

#define CHECK_PTR(x)                                                           \
  if (x == NULL) {                                                             \
    return 1;                                                                  \
  }

int utreexo_forest_modify(struct utreexo_forest *forest,
                          utreexo_node_hash *leaf, int leaf_count) {

  for (int i = 0; i < leaf_count; i++) {
    utreexo_forest_add(forest, leaf[i]);
  }

  return 1;
}
int utreexo_forest_free(struct utreexo_forest *p) {
  _utreexo_forest_free(p);
  return 0;
}

int utreexo_forest_init(struct utreexo_forest **p, const char *filename) {
  CHECK_PTR(p);
  CHECK_PTR(filename);

  struct utreexo_forest *forest = malloc(sizeof(struct utreexo_forest));
  struct utreexo_forest_file *file = NULL;
  char *heap;

  utreexo_forest_file_init(&file, (void **)&heap, filename);

  forest->data = file;
  forest->nLeaf = (uint64_t *)heap;
  forest->roots = (utreexo_forest_node **)(heap + sizeof(uint64_t));

  *p = forest;
  return 0;
}
