cmake_minimum_required(VERSION 3.29)

project(
    SimpleCNN
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(SimpleCNN ${SOURCES})

add_subdirectory(libraries/armadillo-12.8.4)
target_link_libraries(SimpleCNN PUBLIC armadillo)
target_include_directories(SimpleCNN PUBLIC armadillo)