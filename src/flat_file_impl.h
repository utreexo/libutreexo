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

#define MAP_ORIGIN (void *)(1 << 20)

#ifdef TEST
#define MAP_SIZE 1024
#else
#define MAP_SIZE 1024 * 1024 * 1024
#endif

int posix_fallocate(int fd, off_t offset, off_t len);

static inline void utreexo_forest_file_close(struct utreexo_forest_file *file) {
  munmap(file->map - sizeof(struct utreexo_forest_file_header),
         file->header->filesize);
  close(file->fd);
  free(file);
}

static inline void utreexo_forest_file_init(struct utreexo_forest_file **file,
                                            void **heap, const char *filename) {
  debug_print("Openning file %s\n", filename);

  int fd = open(filename, O_RDWR | O_CREAT, 0644);

  if (fd < 0) {
    perror("open");
    exit(1);
  }

  struct utreexo_forest_file *pfile =
      (struct utreexo_forest_file *)malloc(sizeof(struct utreexo_forest_file));
  if (pfile == NULL) {
    perror("malloc");
    exit(1);
  }

  const int fsize = lseek(fd, 0, SEEK_END);

  char *data =
      (char *)mmap(MAP_ORIGIN, MAP_SIZE, PROT_READ | PROT_WRITE | PROT_GROWSUP,
                   MAP_FILE | MAP_SHARED | MAP_FIXED, fd, 0);

  if (data == MAP_FAILED || data != MAP_ORIGIN) {
    perror("mmap");
    exit(1);
  }

  debug_print("File mapped to %p\n", data);
  const size_t header_size = sizeof(struct utreexo_forest_file_header);

  pfile->map = data + header_size;
  pfile->header = (struct utreexo_forest_file_header *)data;
  pfile->filename = filename;
  pfile->fd = fd;

  const struct utreexo_forest_file_header *pheader =
      (struct utreexo_forest_file_header *)data;

  /* This is a new file, we need to initialize at least the first page */
  if (fsize < 4 || pheader->magic != FILE_MAGIC) {
    debug_print("No pages found, creating new file\n");

    posix_fallocate(fd, 0, header_size);

    pfile->header->filesize = header_size;
    pfile->header->n_pages = 0;
    memset(pfile->header->heap, 0x00, HEAP_AREA);
    pfile->header->fpg = NULL;
    pfile->header->magic = FILE_MAGIC;
    pfile->header->wrt_page =
        (struct utreexo_forest_page_header *)(data + header_size);
  }

  if (pheader->n_pages == 0) {
    utreexo_forest_page_alloc(pfile);
  }

  debug_print("Found %d pages writting in %p\n", pfile->header->n_pages,
              pfile->header->wrt_page);
  *file = pfile;
  *heap = pfile->header->heap;
}

static inline int utreexo_forest_page_alloc(struct utreexo_forest_file *file) {
  debug_print("Grabbing a new page\n");
  // We have a free page
  if (file->header->fpg != NULL) {
    debug_print("Found a free page");
    utreexo_forest_free_page *nhead = file->header->fpg->next;
    file->header->wrt_page =
        (struct utreexo_forest_page_header *)file->header->fpg;
    file->header->fpg = nhead;
    file->header->n_pages++;
    return EXIT_SUCCESS;
  }

  // We need to create a new one
  debug_print("Creating a new page\n");

  const int page_offset = file->header->n_pages;
  file->header->n_pages++;
  file->header->filesize += PAGE_SIZE;

  posix_fallocate(file->fd, file->header->filesize,
                  (PAGE_SIZE)*file->header->n_pages);

  char *pg = (((char *)file->map) + PAGE_SIZE * page_offset);
  file->header->wrt_page = (struct utreexo_forest_page_header *)pg;

  utreexo_forest_mkpg(file->header->wrt_page);

  debug_print("Allocated page %d\n", page_offset);
  debug_assert(file->header->wrt_page->n_nodes == 0);
  debug_assert(file->header->wrt_page->pg_magic == MAGIC);
  debug_assert(file->header->n_pages == page_offset + 1);

  return EXIT_SUCCESS;
}

static inline void utreexo_forest_mkpg(struct utreexo_forest_page_header *pg) {
  pg->pg_magic = MAGIC;
  pg->n_nodes = 0;

  debug_print("Initialized page %d\n", file->header->n_pages - 1);
  debug_assert(pg->n_nodes == 0) debug_assert(pg->pg_magic == MAGIC)
}

static inline utreexo_forest_node *
utreexo_forest_file_node_alloc(struct utreexo_forest_file *file) {
  uint64_t page_nodes = file->header->wrt_page->n_nodes;
  if (page_nodes == NODES_PER_PAGE) {
    debug_print("Page is full, allocating new page\n");
    if (utreexo_forest_page_alloc(file)) {
      fprintf(stderr, "Failed to allocate page\n");
      exit(1);
    }
    page_nodes = 0; // we've just created a new page
  }
  debug_print("Writing node %d to page %d offset=%d\n", page_nodes,
              file->header->n_pages - 1,
              page_nodes / sizeof(utreexo_forest_node));
  utreexo_forest_node *ptr =
      (utreexo_forest_node *)((char *)file->header->wrt_page + 16) + page_nodes;
  ++(file->header->wrt_page->n_nodes);
  return ptr;
}

static inline void
utreexo_forest_file_node_del(struct utreexo_forest_file *file,
                             const utreexo_forest_node *node) {
  uint64_t npage = ((uint64_t)((char *)node - file->map)) / PAGE_SIZE;

  struct utreexo_forest_page_header *pg =
      (struct utreexo_forest_page_header *)PAGE(file->map, npage);

  debug_print("Deleting node from page %d remaining: %u\n", npage, pg->n_nodes);
  debug_assert(pg->n_nodes != 0);
  debug_assert(file->header->n_pages > npage);

  if (--pg->n_nodes == 0) {
    debug_print("Deallocating page %d\n", npage);
    --file->header->n_pages;
    utreexo_forest_free_page *pg = file->header->fpg;
    utreexo_forest_free_page *npg =
        (utreexo_forest_free_page *)PAGE(file->map, npage);
    npg->next = NULL;

    // This is the first free page
    if (pg == NULL) {
      file->header->fpg = npg;
      return;
    }
    // Walk the list until find the last element
    while (pg->next != NULL)
      pg = (utreexo_forest_free_page *)pg->next;

    pg->next = npg;
  }
}
#endif
