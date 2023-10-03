#ifndef UTIL_H
#define UTIL_H

#ifdef DEBUG
#define DEBUG_PRINT(...)                                                       \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
  } while (0)
#define DEBUG_ASSERT(x)                                                        \
  if (!(x)) {                                                                  \
    fprintf(stderr, "Assertion failed: %s\n", #x);                             \
    exit(1);                                                                   \
  }
#else
#define DEBUG_PRINT(...)
#define DEBUG_ASSERT(x)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#endif // UTIL_H