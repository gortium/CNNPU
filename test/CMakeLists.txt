cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STNDARD 11)

include_directories("../include")

add_executable(TestCLP TestCLP.cpp)

target_link_libraries(TestCLP gtest_main)