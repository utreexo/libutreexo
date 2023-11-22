#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>

#include "config.h"

#ifdef DEBUG
#define debug_print(...)                                                       \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
  } while (0)
#else
#define debug_print(...)
#endif

#ifdef TEST
#define debug_assert(x)                                                        \
  if (!(x)) {                                                                  \
    fprintf(stderr, "Assertion failed: %s\n", #x);                             \
    exit(1);                                                                   \
  }
#else
#define debug_assert(x)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// detect_row finds the current row of a node, given the position
// and the total forest rows.
static inline int detect_row(uint64_t pos, uint64_t forest_rows) {
  uint64_t marker = 1 << forest_rows;
  uint8_t h = 0;

  while ((pos & marker) != 0) {
    marker >>= 1;
    h += 1;
  }

  return h & 0xff;
}

static inline int tree_rows(uint64_t pos) {
  if (pos == 0)
    return 0;
  unsigned int leading_zeros = 0;
  pos = pos - 1;
  uint64_t bit = (uint64_t)1 << 63;
  while ((pos & bit) == 0) {
    bit >>= 1;
    ++leading_zeros;
  }
  return (64 - leading_zeros) & 0xff;
}

typedef struct {
  uint8_t tree;
  uint8_t depth;
  uint64_t bits;
} node_offset;

static inline node_offset detect_offset(uint64_t pos, uint64_t num_leaves) {
  uint8_t tr = tree_rows(num_leaves);
  uint8_t nr = detect_row(pos, tr);

  uint8_t bigger_trees = tr;
  uint64_t marker = pos;

  while (((marker << nr) & ((2 << tr) - 1)) >= ((1 << tr) & num_leaves)) {
    uint64_t tree_size = (1 << tr) & num_leaves;
    marker -= tree_size;
    bigger_trees -= 1;

    tr -= 1;
  }
  return (node_offset){
      .bits = pos,
      .tree = (uint8_t)bigger_trees,
      .depth = (uint8_t)(tr - nr),
  };
}

#endif // UTIL_H
