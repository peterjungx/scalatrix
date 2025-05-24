cmake_minimum_required(VERSION 3.14)
project(scalatrix_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Declare Catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)

# Make Catch2 available
FetchContent_MakeAvailable(Catch2)


# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../include)

# Test executable
add_executable(test_affine_transform 
    test_affine_transform.cpp
    ${CMAKE_SOURCE_DIR}/../src/params.cpp
    ${CMAKE_SOURCE_DIR}/../src/affine_transform.cpp
    ${CMAKE_SOURCE_DIR}/../src/linear_solver.cpp
)

# Link libraries
target_link_libraries(test_affine_transform 
    Catch2::Catch2WithMain
)

# Enable testing
include(CTest)
include(Catch)
catch_discover_tests(test_affine_transform)