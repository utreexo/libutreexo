/*
 * COPYRIGHT (C) 2023 Davidson Souza. All Rights Reserved.
 *
 * In utreexo, we hold all UTXOs as leaves inside some trees. While we still
 * have trees, they are not sorted by leaf hashes and leaves can move upwards as
 * nodes gets deleted. Thus, making it hard to track individual leaves inside
 * the accumulator. To solve that, we hold a map from leaf_hash -> *leaf, where
 * *leaf is a pointer to that leaf inside the accumulator.
 *
 * When nodes are added and removed, it's only a pointer operation, and no data
 * gets moved around, therefore, it's fair to keep pointers and dereference
 * them to get an (undeleted) node.
 *
 * This is a simple disk-based universal hashing hash map, we allocate a
 * gigantic file at the beginning (64GB) but use a sparse file, where we
 * "pretend" we have 64GB, but the OS doesn't allocate that until we actually
 * use the space. This file starts with zero bytes and grows as we go.
 */
#ifndef LEAF_MAP_H
#define LEAF_MAP_H

#include "forest_node.h"

/* Represents the offset of a leaf inside the file */
typedef unsigned long leaf_offset;
/* The hash function we'll use to hash keys */
typedef leaf_offset (*hashfp)(unsigned char *key);

/* Our leaf map, it's a simple hash map that maps leaf hashes to leaf pointers
 */
typedef struct {
  int fd;
  hashfp hash;
} utreexo_leaf_map;

/* Creates a new leaf_map. This function doesn't allocate any memory, since
 * utreexo_leaf_map isn't particularly big. Filename is the file we'll store
 * stuff in and flags are the flags for that file on our FS
 */
static inline void utreexo_leaf_map_new(utreexo_leaf_map *map,
                                        const char *filename,
                                        const unsigned int flags,
                                        const hashfp hash);

/* Gets a node's reference from the map. You should pass a pointer to a pointer
 * to a utreexo_forest_node. That's because you'll end-up with a
 * utreexo_forest_node*, the actual thing is inside the mmap-ed file, taking it
 * by value would create a copy that isn't what you want.
 */
static inline void utreexo_leaf_map_get(utreexo_leaf_map *map,
                                        utreexo_forest_node **node,
                                        utreexo_leaf_hash leaf);

/* Sets a key to a given pointer */
static inline void utreexo_leaf_map_set(utreexo_leaf_map *map,
                                        utreexo_forest_node *node,
                                        utreexo_leaf_hash hash);
/* Delete a leaf from the map */
static inline void utreexo_leaf_delete(utreexo_leaf_map *map,
                                       utreexo_node_hash hash);

#endif // LEAF_MAP_H
