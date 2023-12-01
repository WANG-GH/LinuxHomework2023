cmake_minimum_required(VERSION 3.14)
project(homework1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# enable_testing()

add_executable(
  homework1_test
  test.cpp
  Wrapper.cpp
)

target_include_directories(
  homework1_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}
)

target_compile_options(homework1_test PRIVATE -g3 -O0 -pthread)

target_link_libraries(
  homework1_test
  GTest::gtest
)

# include(GoogleTest)
# gtest_discover_tests(homework1_test)