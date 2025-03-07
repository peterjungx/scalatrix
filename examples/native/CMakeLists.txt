cmake_minimum_required(VERSION 3.10)
project(native_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the pre-built scalatrix library as an imported target
add_library(scalatrix STATIC IMPORTED)
set_target_properties(scalatrix PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../build/native/libscalatrix.a"  # Path from examples/native/ to scalatrix/build/native/
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/../../include"      # Path to scalatrix/include/
)

# Define the example executable
add_executable(native_example native_example.cpp)
target_link_libraries(native_example PRIVATE scalatrix)