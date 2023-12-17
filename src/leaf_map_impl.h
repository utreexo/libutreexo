#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "forest_node.h"
#include "leaf_map.h"

static inline unsigned int hash(unsigned char value[36]) {
  unsigned long hash = 5381;
  int c;

  for (size_t i = 0; i < 36; ++i)
    hash = ((hash << 5) + hash) + value[i];

  return hash & 0xffffffff;
}

static inline void utreexo_leaf_map_new(utreexo_leaf_map *map,
                                        const char *filename,
                                        const unsigned int flags) {
  int fd = open(filename, flags, 0666);
  if (fd == -1) {
    perror("open");
    abort();
  }

  lseek(fd, 0xff, SEEK_SET);

  char end = 0;
  write(fd, &end, sizeof(char));

  *map = (utreexo_leaf_map){
      .fd = fd,
  };
}

static inline void utreexo_leaf_map_get(utreexo_leaf_map *map,
                                        utreexo_forest_node **node,
                                        utreexo_leaf_hash leaf) {
  utreexo_forest_node *pnode = NULL;
  unsigned char key[36] = {0};

  memmove(key, leaf.hash, 32);

  do {
    unsigned long position = hash(key) * sizeof(void *);
    lseek(map->fd, position, SEEK_SET);
    read(map->fd, &pnode, sizeof(utreexo_forest_node *));
    ++(*(unsigned int *)&key[32]);
  } while (pnode != NULL && memcmp(pnode->hash.hash, leaf.hash, 32) != 0);
  *node = pnode;
}

static inline void utreexo_leaf_map_set(utreexo_leaf_map *map,
                                        utreexo_forest_node *node,
                                        utreexo_leaf_hash leaf) {
  utreexo_forest_node *pnode = NULL;
  unsigned char key[36] = {0};
  unsigned long position = 0;

  memmove(key, leaf.hash, 32);

  do {
    position = hash(key) * sizeof(utreexo_forest_node **);
    lseek(map->fd, position, SEEK_SET);
    read(map->fd, &pnode, sizeof(utreexo_forest_node **));
    ++(*(unsigned int *)&key[32]);
  } while (pnode != NULL);

  assert(pnode == NULL);

  lseek(map->fd, position, SEEK_SET);
  write(map->fd, &node, sizeof(utreexo_forest_node *));
}

static inline void utreexo_leaf_delete(utreexo_leaf_map *map,
                                       utreexo_node_hash leaf) {
  utreexo_forest_node *pnode = NULL;
  unsigned char key[36] = {0};
  unsigned long position = 0;

  memmove(key, leaf.hash, 32);

  do {
    position = hash(key) * sizeof(void *);
    lseek(map->fd, position, SEEK_SET);
    read(map->fd, pnode, sizeof(utreexo_forest_node **));
    ++(*(unsigned int *)&key[32]);
  } while (pnode != NULL && memcmp(pnode->hash.hash, leaf.hash, 32) != 0);

  pnode = NULL;
  lseek(map->fd, position, SEEK_SET);
  write(map->fd, pnode, sizeof(utreexo_forest_node **));
}
