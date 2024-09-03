#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "forest_node.h"
#include "leaf_map.h"

static utreexo_forest_node *utreexo_thumbstone =
    (utreexo_forest_node *)(1 << sizeof(void *));

static const leaf_offset utreexo_leaf_map_get_pos(unsigned int hash) {
  return hash * sizeof(void *);
}

static inline leaf_offset
utreexo_leaf_map_default_hash(unsigned char value[36]) {
  unsigned long hash = 5381;
  int c;

  for (size_t i = 0; i < 36; ++i)
    hash = ((hash << 5) + hash) + value[i];

  return hash & 0xffffffff;
}

static inline void utreexo_leaf_map_new(utreexo_leaf_map *map,
                                        const char *filename,
                                        const unsigned int flags, hashfp hash) {
  int fd = open(filename, flags, 0666);
  if (fd == -1) {
    perror("open");
    abort();
  }

  lseek(fd, 0xff, SEEK_SET);

  char end = 0;
  write(fd, &end, sizeof(char));
  if (hash == NULL)
    hash = utreexo_leaf_map_default_hash;

  *map = (utreexo_leaf_map){
      .fd = fd,
      .hash = hash,
  };
}

static inline void utreexo_leaf_map_get(utreexo_leaf_map *map,
                                        utreexo_forest_node **node,
                                        utreexo_leaf_hash leaf) {
  utreexo_forest_node *pnode = NULL;
  unsigned char key[36] = {0};

  memmove(key, leaf.hash, 32);

  unsigned int hash = map->hash(key);
  leaf_offset position = utreexo_leaf_map_get_pos(hash);

  do {
    position = utreexo_leaf_map_get_pos(hash);
    ++hash;

    lseek(map->fd, position, SEEK_SET);
    read(map->fd, &pnode, sizeof(utreexo_forest_node *));
    if (pnode == utreexo_thumbstone)
      continue; // we found a deleted node, keep looking

    if (pnode == utreexo_thumbstone)
      continue;
    if (pnode == NULL)
      break;
    if (memcmp(pnode->hash.hash, leaf.hash, 32) == 0)
      break;
  } while (1);
  *node = pnode;
}

static inline void utreexo_leaf_map_set(utreexo_leaf_map *map,
                                        utreexo_forest_node *node,
                                        utreexo_leaf_hash leaf) {
  utreexo_forest_node *pnode = NULL;
  unsigned char key[36] = {0};

  memmove(key, leaf.hash, 32);

  unsigned int hash = map->hash(key);
  leaf_offset position = 0;
  do {
    position = utreexo_leaf_map_get_pos(hash);
    ++hash;

    lseek(map->fd, position, SEEK_SET);
    read(map->fd, &pnode, sizeof(utreexo_forest_node **));

    if (pnode == utreexo_thumbstone) {
      continue;
    }

    if (pnode == NULL)
      break;
  } while (1);
  // this node is already here
  if (pnode != NULL && pnode != utreexo_thumbstone)
    return;

  lseek(map->fd, position, SEEK_SET);
  write(map->fd, &node, sizeof(utreexo_forest_node *));
}

static inline void utreexo_leaf_delete(utreexo_leaf_map *map,
                                       utreexo_node_hash leaf) {
  utreexo_forest_node *pnode = NULL;
  unsigned char key[36] = {0};

  memmove(key, leaf.hash, 32);

  unsigned int hash = map->hash(key);
  leaf_offset position = utreexo_leaf_map_get_pos(hash);

  do {
    position = utreexo_leaf_map_get_pos(hash);
    ++hash;

    lseek(map->fd, position, SEEK_SET);
    read(map->fd, &pnode, sizeof(utreexo_forest_node *));

    if (pnode == utreexo_thumbstone) {
      continue;
    }

    // this node doesn't exist
    if (pnode == NULL) {
      break;
    }
    // we found the node
    if (memcmp(pnode->hash.hash, leaf.hash, 32) == 0)
      break;
  } while (1);
  // node not found, return early
  if (pnode == NULL)
    return;

  // We need to mark positions that have been deleted, because otherwise
  // our open hashing alogritm wouldn't see the colliding elements added
  // afterwards.
  pnode = utreexo_thumbstone;
  lseek(map->fd, position, SEEK_SET);
  write(map->fd, &pnode, sizeof(utreexo_forest_node **));
}
