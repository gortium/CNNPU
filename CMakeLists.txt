cmake_minimum_required(VERSION 3.8)
project(CLP)

set(CMAKE_CXX_STNDARD 11)

#add_subdirectory(libs/libfi)

# LibFi
include_directories("libs/libfi/include")

set(HEADER_FILES include)
include_directories(${HEADER_FILES})

set(SOURCE_FILES src/TestCLP.cpp)
add_executable(PE ${SOURCE_FILES})