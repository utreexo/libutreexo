#ifndef CONFIG_H
#define CONFIG_H

/* Define this symbol to build with some extra sanity checks */
// #define TEST

/* Define this symbol to have debug printing (needs access to printf) */
// #define DEBUG

/* The count of nodes in a single page
 *
 * For testing purposes, it may be small, but for production, a bigger
 * value should be taken.
 * */
#define NODES_PER_PAGE 100

/* Where the file map should start
 *
 * We use a fixed mapping, meaning we'll always map the file at the same
 * memory region, even after restarting the system. This saves us offseting
 * computation to recover the actual positions.
 *
 * The default is start at 1048576 (1 << 20 or 0x100000), there's nothing
 * special about this number. You can't take an address too low, as many
 * architectures won't let you use those. If you take an address too high
 * it might interfere with your program's normal stuff like heap and stack
 * area.
 * */
#define MAP_ORIGIN (void *)(1 << 20)

/* The virtual map size
 *
 * mmap let us creating mapings way bigger than the system's physical memory,
 * the only requirement is being able to find a contiguos space inside a
 * program's virtual memory space.
 *
 * In theory we could use non-continguos memory chucks, but it requires more
 * code for allocation, and isn't supported now.
 * */
#define MAP_SIZE 1024 * 1024 * 1024

/* Magic values to check for consistency while running, if we have some weird
 * memory stuff going on, it may corrupt the page header, causing our program
 * to behave weirdely. We use this to make sure our headers are intact */

/* Magic number for each page (hex for PAGE)*/
#define MAGIC 0x45474150

/* Magic number for the file (hex for UTREXO missing one 'E' because don't fit
 * inside an uint64_t)*/
#define FILE_MAGIC 0x5845525455

#endif // CONFIG_H
