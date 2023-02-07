
cmake_minimum_required (VERSION 3.18)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
aux_source_directory(. ALL_SRCS)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Deploy")

add_executable(vrpTest ${ALL_SRCS})

target_link_libraries(vrpTest ${GTEST_BOTH_LIBRARIES} pthread)

gtest_discover_tests(vrpTest)