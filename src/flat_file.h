/**
 * COPYRIGHT (C) 2023 Davidson Souza. All Rights Reserved.
 *
 * Flat file implementation for utreexo forest.
 * This is a simple implementation that uses mmap to map the forest file
 * into memory, using the OS page cache to optimize forest operations.
 * It's meant to be portable, but other OSes will be second class citizens.
 *
 * It works by keeping track of multiple pages, that are allocated as needed.
 * If all nodes in a page are deleted, the page is freed, and kept in a free
 * list. If we need to allocate a new page, we first check the free list. This
 * helps to keep the file size small, and avoid internal fragmentation, also
 * makes allocation and deallocation fast.
 *
 * Since we are mmaping the file, we can't use the actual pointers to the nodes
 * in the forest, since they will change when the file is remapped. Instead, we
 * use offsets from the beginning of the file, and use a helper function to
 * get the real pointer to the node. This is transparent to the caller, but
 * all node pointer outside this module should be relative to the file (e.g if
 * a node holds the reference to another node, it should be the offset of the
 * node in the file, not the current pointer to the node).
 */
#ifndef UTREEXO_FLAT_FILE_H
#define UTREEXO_FLAT_FILE_H

#include <stdint.h>
#include <sys/types.h>

#include "forest_node.h"

#define NODES_PER_ARENA 10

/* The size of a page minus the header */
#define PAGE_DATA_SIZE (NODES_PER_ARENA * sizeof(utreexo_forest_node))
/* The size of a page, including header */
#define PAGE_SIZE (PAGE_DATA_SIZE + sizeof(struct utreexo_forest_page_header))

/* The position of the data in a page */
#define PAGE_DATA(data, n)                                                     \
  ((char *)data) + PAGE_SIZE *n + sizeof(struct utreexo_forest_page_header)
/* Where the page starts */
#define PAGE(data, n) ((char *)data) + PAGE_SIZE *n

/* Magic number for each page */
#define MAGIC 0x45474150
/* Magic number for the file */
#define FILE_MAGIC 0x4f585455

#ifdef DEBUG
#define DEBUG_PRINT(...)                                                       \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
  } while (0)
#define ASSERT(x)                                                              \
  if (!(x)) {                                                                  \
    fprintf(stderr, "Assertion failed: %s\n", #x);                             \
    exit(1);                                                                   \
  }
#else
#define DEBUG_PRINT(...)
#define ASSERT(x)
#endif

struct utreexo_forest_free_page {
  uint64_t pos;
  struct utreexo_forest_free_page *next;
};

struct utreexo_forest_free_list {
  struct utreexo_forest_free_page *head;
  struct utreexo_forest_free_page *tail;
};

struct utreexo_forest_page_header {
  /* Used for detecting corruption */
  uint64_t pg_magic;
  uint64_t n_nodes;
};

struct utreexo_forest_file {
  const char *filename;
  char *map;
  struct utreexo_forest_page_header *wrt_page;
  int fd;
  uint64_t filesize;
  uint32_t n_pages;
  struct utreexo_forest_free_list free_list;
};

/* Close the file, and free the memory */
static inline void utreexo_forest_file_close(struct utreexo_forest_file *file);

/* Initialize the file, and map it to memory. Creates the file if it doesn't
 * exist */
static inline void utreexo_forest_file_init(struct utreexo_forest_file **file,
                                            const char *filename);
/* Gets a real reference to the node */
static inline void
utreexo_forest_file_node_get(struct utreexo_forest_file *file,
                             utreexo_forest_node **node,
                             utreexo_forest_node_ptr ptr);
/* Puts a new node in the file, returns the offset for future referencing */
static inline void
utreexo_forest_file_node_put(struct utreexo_forest_file *file,
                             utreexo_forest_node_ptr *offset,
                             utreexo_forest_node node);

/* Initialize a new page */
static inline void utreexo_forest_mkpg(struct utreexo_forest_file *file,
                                       struct utreexo_forest_page_header *pg);

static inline int utreexo_forest_page_alloc(struct utreexo_forest_file *file);
#endif