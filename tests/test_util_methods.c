#include <stdio.h>
#include <stdlib.h>

#include "test_utils.h"
#include <util.h>

void test_tree_rows() {
  ASSERT_EQ(tree_rows(8), 3);
  ASSERT_EQ(tree_rows(9), 4);
  ASSERT_EQ(tree_rows(12), 4);
  ASSERT_EQ(tree_rows(255), 8);
};

void test_detect_row() {
  ASSERT_EQ(detect_row(0, 3), 0);
  ASSERT_EQ(detect_row(10, 3), 1);
  ASSERT_EQ(detect_row(2, 1), 1);
  ASSERT_EQ(detect_row(13, 3), 2);
}

void test_detect_offset() {
  node_offset offset = detect_offset(0, 3);
  ASSERT_EQ(offset.depth, 1);
  ASSERT_EQ(offset.tree, 1);
  ASSERT_EQ(offset.bits, 1);
}

int main() {
  test_tree_rows();
  test_detect_row();
  test_detect_offset();
}
