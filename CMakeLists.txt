cmake_minimum_required(VERSION 3.8)

project(CNNPU)

set(CMAKE_CXX_FLAGS "-fpermissive -std=c++11")

include_directories("lib/libfi/include")
add_subdirectory(lib/googletest)
add_subdirectory(src)
add_subdirectory(test)