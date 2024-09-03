AUTOMAKE_OPTIONS = foreign

SUBDIRS = include

#lib_LTLIBRARIES = libutreexo_cpp.la
#libutreexo_cpp_la_SOURCES = include/cpp/utreexo.cpp
#libutreexo_cpp_la_LDFLAGS = -version-info 0:1:0

check_PROGRAMS = test_flat_file test_forest test_leaf_map test_utils

test_flat_file_SOURCES = tests/test_flat_file.c

test_leaf_map_SOURCES = tests/test_leaf_map.c

test_forest_SOURCES = tests/test_forest.c
test_forest_LDADD = libutreexo.la -lcrypto

test_utils_SOURCES = src/util.h tests/test_util_methods.c

lib_LTLIBRARIES = libutreexo.la
libutreexo_la_SOURCES = src/mmap_forest.c
