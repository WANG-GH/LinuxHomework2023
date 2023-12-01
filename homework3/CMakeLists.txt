cmake_minimum_required(VERSION 3.10)
project(homework3)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 查找gtest库
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# 添加测试
add_executable(homework3_test test.cpp spliter.cpp)
target_compile_options(homework3_test PRIVATE -g3 -O0 -pthread)
target_link_libraries(homework3_test ${GTEST_LIBRARIES} pthread)