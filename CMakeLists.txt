
cmake_minimum_required(VERSION 3.12)
project (dimacsVRP)

aux_source_directory(${PROJECT_SOURCE_DIR}/src src_dirs)

# 条件判断
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0")
    message("Debug mode:${CMAKE_C_FLAGS_DEBUG}")

elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
    message("Release mode:${CMAKE_C_FLAGS_RELEASE}")
else()
    message("else:${CMAKE_BUILD_TYPE}")
    message("else:${CMAKE_C_FLAGS_RELEASE}")
endif()

add_subdirectory(Source)
