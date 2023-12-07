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

#define CHECK_PTR_VAR(x, n)                                                    \
  if (n > 0 && x == NULL)                                                      \
    return -1;

extern int utreexo_forest_modify(struct utreexo_forest *forest,
                                 utreexo_node_hash *utxos, int utxo_count,
                                 utreexo_node_hash *stxos, int stxo_count) {
  CHECK_PTR(forest);
  CHECK_PTR_VAR(utxos, utxo_count);
  CHECK_PTR_VAR(stxos, stxo_count);

  for (size_t utxo = 0; utxo < utxo_count; ++utxo) {
    utreexo_forest_node *pnode = NULL;
    utreexo_leaf_map_get(&forest->leaf_map, &pnode, utxos[utxo]);
    if (pnode == NULL)
      return -3;
    if (delete_single(forest, pnode)) {
      return -2;
    }
  }

  for (size_t i = 0; i < utxo_count; i++) {
    utreexo_forest_add(forest, utxos[i]);
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
