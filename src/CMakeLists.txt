cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STNDARD 11)

include_directories("../include")

add_executable(CNNPU CNNP/main.cpp)