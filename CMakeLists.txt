cmake_minimum_required (VERSION 2.8)
project (DERMIS)

set (DERMIS_VERSION_MAJOR 0)
set (DERMIS_VERSION_MINOR 1)

#set (CMAKE_C_FLAGS "-Wall -Werror -Wpedantic -std=c11")
#set (CMAKE_C_FLAGS "-g -Wall -Wpedantic -std=c11 -m32")
set (CMAKE_C_FLAGS "-g -Wall -Werror -std=c11")

if (UNIX)
    set (CMAKE_POSITION_INDEPENDENT_CODE ON)
endif(UNIX)

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(examples)
