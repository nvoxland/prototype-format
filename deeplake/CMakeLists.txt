cmake_minimum_required(VERSION 3.26)
project(deeplake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest json)

enable_testing()

add_library(deeplake dataset.cpp)
add_executable(
        runTests
        dataset.test.cpp
)

target_link_libraries(runTests deeplake GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(runTests)