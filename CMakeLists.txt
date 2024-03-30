cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/myunittests")

# Get all .cc files in the TEST_DIR directory
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
include(GoogleTest)
foreach(src_file ${SRC_FILES})
  # Get file name without directory and extension
  get_filename_component(src_name ${src_file} NAME_WE)

  # Check if the test file exists
  set(test_file "${TEST_DIR}/${src_name}_test.cc")
  if(EXISTS ${test_file})
    # Add the source file and the test file to the executable
    add_executable(${src_name}_test ${src_file} ${test_file})

    target_link_libraries(${src_name}_test GTest::gtest_main)
    gtest_discover_tests(${src_name}_test)
  endif()
endforeach()