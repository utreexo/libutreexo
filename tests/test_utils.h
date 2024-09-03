#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    if (a != b) {                                                              \
      printf("Assertion failed: %s != %s\n", #a, #b);                          \
      printf("  %s = %016lx\n", #a, (unsigned long)a);                         \
      printf("  %s = %016lx\n", #b, (unsigned long)b);                         \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define ASSERT_ARRAY_EQ(a, b, len)                                             \
  do {                                                                         \
    for (size_t i = 0; i < len; i++) {                                         \
      ASSERT_EQ(a[i], b[i]);                                                   \
    }                                                                          \
  } while (0)

#define TEST_BEGIN(name)                                                       \
  do {                                                                         \
    printf("Running %s...", #name);                                            \
  } while (0)

#define TEST_END                                                               \
  do {                                                                         \
    printf("OK\n");                                                            \
  } while (0)
