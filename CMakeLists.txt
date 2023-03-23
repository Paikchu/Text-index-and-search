cmake_minimum_required(VERSION 3.24)
project(Test)

set(CMAKE_CXX_STANDARD 17)

add_executable(indexer indexer.cpp)
add_executable(searcher searcher.cpp)

find_package(XAPIAN REQUIRED)
include_directories(${XAPIAN_INCLUDE_DIR})
target_link_libraries(indexer ${XAPIAN_LIBRARIES})
target_link_libraries(searcher ${XAPIAN_LIBRARIES})
#
#add_executable(test test.cpp)