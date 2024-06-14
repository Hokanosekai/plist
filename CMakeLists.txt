cmake_minimum_required(VERSION 3.10)
project(PList C)

set(CMAKE_C_STANDARD 99)

# Add ou logic code file
add_executable(PList src/main.c src/utils.c src/plist.c)

# Includes all header file as the src folder
target_include_directories(PList PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Strip le binaire
target_link_options(PList PRIVATE -s)

