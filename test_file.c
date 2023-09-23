#include "src/flat_file.h"
#include "src/flat_file_impl.h"
#include "src/map_forest.h"
#include "src/parent_hash.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const static char expected_hash[][32] = {{0x00, 0x01, 0x02, 0x03},
                                         {0x00, 0x01, 0x02, 0x04},
                                         {0x00, 0x01, 0x02, 0x05}};
// asserts if two hashes are equal
#define ASSERT_EQ(a, b) ASSERT(memcmp(a, b, 32) == 0)

utreexo_forest_node_ptr test_create_nodes(struct utreexo_forest_file *file);

void test_retrieve_nodes(struct utreexo_forest_file *file,
                         utreexo_forest_node_ptr parent);

void test_delete_nodes(struct utreexo_forest_file *file,
                       utreexo_forest_node_ptr parent_pos);

void test_add_many(int n_adds);

int main() {
  struct utreexo_forest_file *file;
  utreexo_forest_file_init(&file, "test.dat");

  const utreexo_forest_node_ptr parent = test_create_nodes(file);
  test_retrieve_nodes(file, parent);
  test_delete_nodes(file, parent);
  utreexo_forest_file_close(file);
  test_add_many(NODES_PER_ARENA * 200);
  return 0;
}
void print_hash(utreexo_node_hash hash) {
  for (int i = 0; i < 32; i++) {
    printf("%02x", hash.hash[i]);
  }
  printf("\n");
}

utreexo_forest_node_ptr test_create_nodes(struct utreexo_forest_file *file) {
  utreexo_forest_node_ptr right_child_pos;
  utreexo_forest_node_ptr left_child_pos;
  utreexo_forest_node_ptr parent_pos;

  utreexo_forest_node right_child = {
      .hash = {0x00, 0x01, 0x02, 0x05},
      .left_child = 0,
      .right_child = 0,
      .parent = 0,
  };
  utreexo_forest_file_node_put(file, &right_child_pos, right_child);
  utreexo_forest_node left_child = {
      .hash = {0x00, 0x01, 0x02, 0x04},
      .left_child = 0,
      .right_child = 0,
      .parent = 0,
  };

  utreexo_forest_file_node_put(file, &left_child_pos, left_child);
  utreexo_forest_node parent = {
      .hash = {0x00, 0x01, 0x02, 0x03},
      .parent = 0,
      .left_child = left_child_pos,
      .right_child = right_child_pos,
  };
  utreexo_forest_file_node_put(file, &parent_pos, parent);
  return parent_pos;
}
void test_delete_nodes(struct utreexo_forest_file *file,
                       utreexo_forest_node_ptr parent_pos) {
  utreexo_forest_node *parent = NULL;
  utreexo_forest_file_node_get(file, &parent, parent_pos);

  utreexo_forest_node *left_child = NULL;
  utreexo_forest_file_node_get(file, &left_child, parent->left_child);
  utreexo_forest_file_node_del(file, parent->left_child);

  utreexo_forest_node *right_child = NULL;
  utreexo_forest_file_node_get(file, &right_child, parent->right_child);
  utreexo_forest_file_node_del(file, parent->right_child);

  utreexo_forest_file_node_del(file, parent_pos);
}

void test_retrieve_nodes(struct utreexo_forest_file *file,
                         utreexo_forest_node_ptr parent_pos) {
  utreexo_forest_node *parent = NULL;
  utreexo_forest_file_node_get(file, &parent, parent_pos);
  ASSERT_EQ(parent->hash.hash, expected_hash[0]);

  utreexo_forest_node *left_child = NULL;
  utreexo_forest_file_node_get(file, &left_child, parent->left_child);
  ASSERT_EQ(left_child->hash.hash, expected_hash[1]);

  utreexo_forest_node *right_child = NULL;
  utreexo_forest_file_node_get(file, &right_child, parent->right_child);
  ASSERT_EQ(right_child->hash.hash, expected_hash[2]);
}

void test_add_many(int n_adds) {
  struct utreexo_forest_file *file;
  utreexo_forest_file_init(&file, "add_many.dat");
  utreexo_forest_node_ptr node_pos;
  utreexo_forest_node node = {
      .hash = {0xff, 0xff, 0xff, 0xff},
      .left_child = {},
      .right_child = {},
      .parent = {},
  };
  for (int i = 0; i < n_adds; i++) {
    utreexo_forest_file_node_put(file, &node_pos, node);
  }
  utreexo_forest_file_close(file);
}
