cmake_minimum_required(VERSION 3.10)
project(wasm_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Eigen3_DIR "/opt/homebrew/Cellar/eigen/3.4.0_1/share/eigen3/cmake")
find_package(Eigen3 REQUIRED)

add_library(scalatrix_wasm STATIC IMPORTED)
set_target_properties(scalatrix_wasm PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../build/wasm/scalatrix.js"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/../../include"
)

add_custom_target(wasm_example ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/../../build/wasm/scalatrix.js
        ${CMAKE_CURRENT_SOURCE_DIR}/wasm_example.html
        ${CMAKE_CURRENT_SOURCE_DIR}/wasm_example.js
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying Wasm example files to build directory"
)