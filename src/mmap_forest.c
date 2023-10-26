static const int utreexo_forest_version_patch = 0;
static const int utreexo_forest_version_minor = 1;
static const int utreexo_forest_version_major = 0;

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mmap_forest.h"
#include "flat_file.h"
#include "util.h"
#include "map_forest_impl.h"

void utreexo_forest_modify(struct utreexo_forest *forest,
                           utreexo_node_hash *leaf, int leaf_count) {
  for (int i = 0; i < leaf_count; i++) {
    utreexo_forest_add(forest, leaf[i]);
  }
}

void utreexo_forest_init(struct utreexo_forest **p, const char *filename) {
  struct utreexo_forest *forest = malloc(sizeof(struct utreexo_forest));
  struct utreexo_forest_file *file = NULL;
  void *roots = NULL;
  utreexo_forest_file_init(&file, filename, &roots);
  forest->data = file;
  forest->nLeaf = 0;
  (*p)->roots = roots;
  DEBUG_ASSERT(*(uint32_t *)(*file).map == MAGIC);
  *p = forest;
}

void utreexo_forest_free(struct utreexo_forest *forest) {
  utreexo_forest_file_close(forest->data);
  free(forest);
}
