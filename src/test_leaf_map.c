#include "flat_file_impl.h"
#include "leaf_map.h"
#include "leaf_map_impl.h"
#include "mmap_forest.h"
#include "test_utils.h"
#include <stdio.h>

leaf_offset chash(unsigned char value[36]) { return value[32]; }

int main() {
  {
    TEST_BEGIN("add one");
    struct utreexo_forest_file *file = NULL;
    void *_ptr;
    utreexo_leaf_map map;

    utreexo_leaf_map_new(&map, "leaf_map_leaves1.bin", O_CREAT | O_RDWR, chash);
    utreexo_forest_file_init(&file, &_ptr, "leaf_map_test_map1.bin");

    utreexo_forest_node *n = utreexo_forest_file_node_alloc(file);
    utreexo_leaf_map_set(&map, n, (utreexo_leaf_hash){.hash = {1}});
    n->hash = (utreexo_leaf_hash){.hash = {1}};

    utreexo_forest_node *n1 = NULL;
    utreexo_leaf_map_get(&map, &n1, (utreexo_leaf_hash){.hash = {1}});

    ASSERT_EQ(n, n1);
    TEST_END;
  }
  {
    TEST_BEGIN("add with collision");
    struct utreexo_forest_file *file = NULL;
    void *_ptr;
    utreexo_leaf_map map;

    utreexo_leaf_map_new(&map, "leaf_map_leaves2.bin", O_CREAT | O_RDWR, chash);
    utreexo_forest_file_init(&file, &_ptr, "leaf_map_test_map2.bin");

    // alloc a new node
    utreexo_forest_node *n = utreexo_forest_file_node_alloc(file);
    n->hash = (utreexo_leaf_hash){.hash = {1}};

    // two duplicates
    utreexo_leaf_map_set(&map, n, (utreexo_leaf_hash){.hash = {1}});
    utreexo_leaf_map_set(&map, n, (utreexo_leaf_hash){.hash = {1}});

    // another hash
    utreexo_forest_node *np = utreexo_forest_file_node_alloc(file);
    np->hash = (utreexo_leaf_hash){.hash = {2}};
    utreexo_leaf_map_set(&map, np, (utreexo_leaf_hash){.hash = {2}});

    // try getting the first node back
    utreexo_forest_node *n1 = NULL;
    utreexo_leaf_map_get(&map, &n1, (utreexo_leaf_hash){.hash = {1}});
    ASSERT_EQ(n, n1);

    // delete both nodes
    utreexo_leaf_map_delete(&map, (utreexo_leaf_hash){.hash = {1}});
    utreexo_leaf_map_delete(&map, (utreexo_leaf_hash){.hash = {2}});

    TEST_END;
  }
  {
    // this is basically a block worth of utxos
    TEST_BEGIN("add twenty thousand nodes and delete them all");
    struct utreexo_forest_file *file = NULL;
    void *_ptr;
    utreexo_leaf_map map;

    utreexo_leaf_map_new(&map, "leaf_map_leaves3.bin", O_CREAT | O_RDWR, NULL);
    utreexo_forest_file_init(&file, &_ptr, "leaf_map_test_map3.bin");

    for (size_t i = 0; i < 20000; ++i) {
      utreexo_forest_node *n = utreexo_forest_file_node_alloc(file);
      memmove(&n->hash.hash, &i, sizeof(size_t));
      utreexo_leaf_map_set(&map, n, n->hash);
    }

    for (size_t i = 0; i < 20000; ++i) {
      utreexo_leaf_hash hash;
      memmove(hash.hash, &i, sizeof(size_t));

      utreexo_forest_node *n = NULL;
      utreexo_leaf_map_get(&map, &n, hash);
      assert(n != NULL);
      utreexo_forest_file_node_del(file, n);
    }

    TEST_END;
  }
  {
    TEST_BEGIN("add one thousand, delete one and try to get it back");
    struct utreexo_forest_file *file = NULL;
    void *_ptr;
    utreexo_leaf_map map;
    utreexo_leaf_map_new(&map, "leaf_map_leaves4.bin", O_CREAT | O_RDWR, NULL);
    utreexo_forest_file_init(&file, &_ptr, "leaf_map_test_map4.bin");

    for (size_t i = 0; i < 1000; ++i) {
      utreexo_forest_node *n = utreexo_forest_file_node_alloc(file);
      memmove(&n->hash.hash, &i, sizeof(size_t));
      utreexo_leaf_map_set(&map, n, n->hash);
    }

    utreexo_leaf_hash del_hash;
    size_t i = 1;
    memmove(del_hash.hash, &i, sizeof(size_t));

    utreexo_forest_node *n = NULL;
    utreexo_leaf_map_get(&map, &n, del_hash);
    assert(n != NULL);
    utreexo_leaf_map_delete(&map, del_hash);

    utreexo_leaf_map_get(&map, &n, del_hash);
    assert(n == NULL);
    TEST_END;
  }
}
