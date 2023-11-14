#ifndef UTREEXO_FLAT_FILE_IMPL_H
#define UTREEXO_FLAT_FILE_IMPL_H

#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "flat_file.h"
#include "forest_node.h"
#include "util.h"

#ifndef TEST
#define MAP_ORIGIN (void *)(1 << 10)
#else
#define MAP_ORIGIN NULL
#endif

#ifdef TEST
#define MAP_SIZE 1024
#else
#define MAP_SIZE 1024 * 1024 * 1024
#endif

int posix_fallocate(int fd, off_t offset, off_t len);

static inline void utreexo_forest_file_close(struct utreexo_forest_file *file) {
  munmap(file->map, file->filesize);
  close(file->fd);
  free(file);
}

static inline void utreexo_forest_file_init(struct utreexo_forest_file **file,
                                            const char *filename) {

  int fd = 0;

  *file =
      (struct utreexo_forest_file *)malloc(sizeof(struct utreexo_forest_file));

  fd = open(filename, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    perror("open");
    exit(1);
  }

  const int fsize = lseek(fd, 0, SEEK_END);

  char *data =
      (char *)mmap(MAP_ORIGIN, MAP_SIZE, PROT_READ | PROT_WRITE | PROT_GROWSUP,
                   MAP_FILE | MAP_SHARED, fd, 0);

  if (data == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  (*file)->wrt_page = (struct utreexo_forest_page_header *)(data + sizeof(int));

  (*file)->fd = fd;
  (*file)->filename = filename;
  (*file)->filesize = fsize;
  (*file)->n_pages = 0;
  (*file)->map = data + sizeof(int);
  (*file)->fpg = NULL;

  /* This is a new file, we need to initialize at least the first page */
  if (fsize < 4 || *(int *)data != FILE_MAGIC) {
    DEBUG_PRINT("No pages found, creating new file\n");
    posix_fallocate(fd, 0, 4);
    utreexo_forest_page_alloc(*file);
    *(int *)data = FILE_MAGIC;
    return;
  }
  (*file)->n_pages = 1;
  (*file)->wrt_page = (struct utreexo_forest_page_header *)(data + 4);
  DEBUG_PRINT("Found %d pages\n", (*file)->n_pages);
}
static inline int utreexo_forest_page_alloc(struct utreexo_forest_file *file) {
  DEBUG_PRINT("Grabbing a new page\n");
  if (file->fpg != NULL) {
    DEBUG_PRINT("Found a free page");
    utreexo_forest_free_page *nhead = file->fpg->next;
    file->wrt_page = (struct utreexo_forest_page_header *)file->fpg;
    file->fpg = nhead;
    return EXIT_SUCCESS;
  }
  DEBUG_PRINT("Creating a new page\n");
  const int page_offset = file->n_pages;
  file->n_pages++;
  file->filesize += PAGE_SIZE;
  posix_fallocate(file->fd, file->filesize, (PAGE_SIZE)*file->n_pages);

  char *pg = (((char *)file->map) + PAGE_SIZE * page_offset);
  file->wrt_page = (struct utreexo_forest_page_header *)pg;

  utreexo_forest_mkpg(file, file->wrt_page);
  DEBUG_PRINT("Allocated page %d\n", page_offset);
  DEBUG_ASSERT(file->wrt_page->n_nodes == 0)
  DEBUG_ASSERT(file->wrt_page->pg_magic == MAGIC)
  DEBUG_ASSERT(file->n_pages == page_offset + 1)
  return EXIT_SUCCESS;
}

static inline void utreexo_forest_mkpg(struct utreexo_forest_file *file,
                                       struct utreexo_forest_page_header *pg) {
  pg->pg_magic = MAGIC;
  pg->n_nodes = 0;

  DEBUG_PRINT("Initialized page %d\n", file->n_pages - 1);
  DEBUG_ASSERT(pg->n_nodes == 0)
  DEBUG_ASSERT(pg->pg_magic == MAGIC)
}

static inline utreexo_forest_node *
utreexo_forest_file_node_alloc(struct utreexo_forest_file *file) {
  uint64_t page_nodes = file->wrt_page->n_nodes;
  if (page_nodes == NODES_PER_PAGE) {
    DEBUG_PRINT("Page is full, allocating new page\n");
    if (utreexo_forest_page_alloc(file)) {
      fprintf(stderr, "Failed to allocate page\n");
      exit(1);
    }
    page_nodes = 0; // we've just created a new page
  }
  DEBUG_PRINT("Writing node %d to page %d offset=%d\n", page_nodes,
              file->n_pages - 1, page_nodes / sizeof(utreexo_forest_node));
  utreexo_forest_node *ptr =
      (utreexo_forest_node *)((char *)file->wrt_page + 16) + page_nodes;
  ++(file->wrt_page->n_nodes);
  return ptr;
}

static inline void
utreexo_forest_file_node_del(struct utreexo_forest_file *file,
                             const utreexo_forest_node *node) {
  uint64_t npage = ((uint64_t)((char *)node - file->map)) / PAGE_SIZE;

  struct utreexo_forest_page_header *pg =
      (struct utreexo_forest_page_header *)PAGE(file->map, npage);

  DEBUG_PRINT("Deleting node from page %d remaining: %u\n", npage, pg->n_nodes);
  DEBUG_ASSERT(pg->n_nodes != 0);
  DEBUG_ASSERT(file->n_pages >= npage);

  if (--pg->n_nodes == 0) {
    DEBUG_PRINT("Deallocating page %d\n", npage);
    utreexo_forest_free_page *pg = file->fpg;
    // This is the first free page
    if (pg == NULL) {
      file->fpg = (utreexo_forest_free_page *)PAGE(file->map, npage);
      return;
    }
    // Walk the list until find the last element
    while (pg->next != NULL)
      pg = (utreexo_forest_free_page *)pg->next;

    pg->next = (utreexo_forest_free_page *)PAGE(file->map, npage);
  }
}
#endif
