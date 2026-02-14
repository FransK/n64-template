cmake_minimum_required(VERSION 3.14)
project(n64-template-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_SOURCES test/Test*.cpp)

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Coverage support
option(COVERAGE "Enable code coverage" OFF)
if(COVERAGE)
    target_compile_options(tests PRIVATE --coverage -O0 -g)
    target_link_options(tests PRIVATE --coverage)
endif()

include(Catch2Testing OPTIONAL)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch OPTIONAL)
if(COMMAND catch_discover_tests)
    catch_discover_tests(tests)
endif()
