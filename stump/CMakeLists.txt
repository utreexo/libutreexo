# CMake minimum required version
cmake_minimum_required(VERSION 3.10)

# Project name and type
project(stump C)

# Specify the C standard
set(CMAKE_C_STANDARD 99)

# Include header files directory
include_directories(include)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Include directories for OpenSSL
include_directories(${OPENSSL_INCLUDE_DIR})

# Add the executable by specifying source files
add_executable(stump src/main.c src/util.c src/stump.c)

# Link OpenSSL libraries
target_link_libraries(stump ${OPENSSL_LIBRARIES})
