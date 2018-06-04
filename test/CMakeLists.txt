cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STNDARD 11)

include_directories("../include")

add_executable(TestCE TestControllerAndCE.cpp)
add_executable(TestPE TestPE.cpp)
add_executable(TestMPE TestMPE.cpp)
add_executable(TestControllerAndCE TestControllerAndCE.cpp)

target_link_libraries(TestCE gtest_main)
target_link_libraries(TestPE gtest_main)
target_link_libraries(TestMPE gtest_main)
target_link_libraries(TestControllerAndCE gtest_main)
