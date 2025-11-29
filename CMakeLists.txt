# Compiler settings
cmake_minimum_required(VERSION 3.15)
project(Wyvern)
set(CMAKE_CXX_STANDARD 17)

# Put all final binaries in bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add subdirectories
add_subdirectory(wyvern)
add_subdirectory(testapp)