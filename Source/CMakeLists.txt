
cmake_minimum_required (VERSION 3.18)

aux_source_directory(. ALL_SRCS)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Deploy")

add_executable(DLLSMA ${ALL_SRCS})
