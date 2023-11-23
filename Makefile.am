AUTOMAKE_OPTIONS = foreign

SUBDIRS = include

check_PROGRAMS = test_flat_file test_forest
test_flat_file_SOURCES = src/test_flat_file.c 

test_forest_SOURCES = src/forest_test.c
test_forest_LDADD = libutreexo.la -lcrypto

lib_LTLIBRARIES = libutreexo.la
libutreexo_la_SOURCES = src/mmap_forest.c
