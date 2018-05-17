cmake_minimum_required(VERSION 3.8)

project(CNNP)

#set(CMAKE_CXX_STNDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

include_directories("lib/libfi/include")
add_subdirectory(lib/googletest)
add_subdirectory(src)
add_subdirectory(test)