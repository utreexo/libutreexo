#include "flat_file_impl.h"
#include "leaf_map.h"
#include "leaf_map_impl.h"
#include "test_utils.h"

int main() {
  struct utreexo_forest_file *file = NULL;
  void *_ptr;
  utreexo_leaf_map map;

  utreexo_leaf_map_new(&map, "leaves.bin", O_CREAT | O_RDWR);
  utreexo_forest_file_init(&file, &_ptr, "test_map.bin");

  utreexo_forest_node *n = utreexo_forest_file_node_alloc(file);
  utreexo_leaf_map_set(&map, n, (utreexo_leaf_hash){.hash = {1}});
  n->hash = (utreexo_leaf_hash){.hash = {1}};

  utreexo_forest_node *n1 = NULL;
  utreexo_leaf_map_get(&map, &n1, (utreexo_leaf_hash){.hash = {1}});

  ASSERT_EQ(n, n1);
}
