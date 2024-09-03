AUTOMAKE_OPTIONS = foreign

SUBDIRS = include

#lib_LTLIBRARIES = libutreexo_cpp.la
#libutreexo_cpp_la_SOURCES = include/cpp/utreexo.cpp
#libutreexo_cpp_la_LDFLAGS = -version-info 0:1:0

check_PROGRAMS = test_flat_file test_forest test_leaf_map

test_flat_file_SOURCES = src/test_flat_file.c

test_leaf_map_SOURCES = src/test_leaf_map.c

test_forest_SOURCES = src/forest_test.c
test_forest_LDADD = libutreexo.la -lcrypto

lib_LTLIBRARIES = libutreexo.la
libutreexo_la_SOURCES = src/mmap_forest.c
