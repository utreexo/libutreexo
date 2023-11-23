AC_PREREQ([2.71])

# The library version is based on libtool versioning of the ABI. The set of
# rules for updating the version can be found here:
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
# All changes in experimental modules are treated as if they don't affect the
# interface and therefore only increase the revision.
define(_LIB_VERSION_CURRENT, 3)
define(_LIB_VERSION_REVISION, 1)
define(_LIB_VERSION_AGE, 1)

AC_INIT([mmap_forest],[1.0],[https://github.com/Davidson-Souza/mmap-forest/issues],[github.com/Davidson-Souza/mmap-forest])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AC_CANONICAL_HOST

# Require Automake 1.11.2 for AM_PROG_AR
AM_INIT_AUTOMAKE([1.11.2 foreign subdir-objects])

# Make the compilation flags quiet unless V=1 is used.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_AS
AM_PROG_AR

# Clear some cache variables as a workaround for a bug that appears due to a bad
# interaction between AM_PROG_AR and LT_INIT when combining MSVC's archiver lib.exe.
# https://debbugs.gnu.org/cgi/bugreport.cgi?bug=54421
AS_UNSET(ac_cv_prog_AR)
AS_UNSET(ac_cv_prog_ac_ct_AR)
LT_INIT([win32-dll])

AC_CHECK_HEADERS([openssl/crypto.h], [], [AC_MSG_ERROR([openssl not found!])])


NODES_PER_PAGE=1024
MAP_ORIGIN=1048576 # (1 << 20) nothing special about this number
MAP_SIZE=107374182400 # 100 GB
MAGIC=0x45474150
FILE_MAGIC=0x5845525455

AC_ARG_WITH(nodes-per-page,
              [AS_HELP_STRING([--with-nodes-per-page=n],["Set the number of nodes per arena (default is 1024")])], [NODES_PER_PAGE=$withval])

AC_ARG_WITH(map-origin, 
            [AS_HELP_STRING([--map-origin=n], 
                            ["Where the map should start. We use fixed map for our file, meaning that it'll always map to the same position, even after a system reboot. Should be a number small enought to not interfere with the program's default memory regions, but not tool small or any sane MMU won't let us map there"])], 
            [MAP_ORIGIN=$withval])

AC_ARG_WITH(map-size, 
            [AS_HELP_STRING([--map-size=n], 
                            ["Defines the size of our mapping (in bytes). We use a continguos memory region, so we need the whole space (MAP_ORIGIN, MAP_ORIGIN + MAP_SIZE) to be available. Default is 100GB"])], 
            [MAP_ORIGIN=$withval])

AC_ARG_WITH(page-magic, 
            [AS_HELP_STRING([--page-magic], 
                            ["Set the magic value that comes at every page's beggining. This value is used to check against corruption and can be any 4 bytes integer. Default is 0x45474150 with is the hexadecimal for PAGE"])],
            [MAGIC=$withval])

AC_ARG_WITH(file-magic, 
            [AS_HELP_STRING([--file-magic], 
                            ["Set the magic value that comes in every file. This value is used to check against corruption and detect files we can read, may be any 8 bytes integer. Default is 0x5845525455, hexadecimal for UTREXO"])],
            [MAGIC=$withval])


AC_DEFINE_UNQUOTED([NODES_PER_PAGE], [$NODES_PER_PAGE], [Number of nodes per arena])
AC_DEFINE_UNQUOTED([MAP_ORIGIN], [$MAP_ORIGIN], [Where we should start our mapping])
AC_DEFINE_UNQUOTED([MAP_SIZE], [$MAP_SIZE], [The size of our mapping])
AC_DEFINE_UNQUOTED([MAGIC], [$MAGIC], [Magic value use to detect page corruption])
AC_DEFINE_UNQUOTED([FILE_MAGIC], [$FILE_MAGIC], [Magic value used to check if the file is corrupted or uninitialized])

###
### Generate output
###

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([include/Makefile])

AC_LANG([C++])
AC_PROG_CXX

AC_OUTPUT
