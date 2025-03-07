cmake_minimum_required(VERSION 3.10)
project(scalatrix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Eigen3_DIR "/opt/homebrew/Cellar/eigen/3.4.0_1/share/eigen3/cmake")
find_package(Eigen3 REQUIRED)

set(SOURCES
    src/affine_transform.cpp
    src/scale.cpp
    src/lattice.cpp
    src/params.cpp
    src/main.cpp  # Ensure included
)

add_library(scalatrix STATIC ${SOURCES})
target_include_directories(scalatrix PUBLIC include)
target_link_libraries(scalatrix PRIVATE Eigen3::Eigen)
set_target_properties(scalatrix PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/native
)

if (EMSCRIPTEN)
    add_executable(scalatrix_wasm ${SOURCES})
    target_include_directories(scalatrix_wasm PUBLIC include)
    target_link_libraries(scalatrix_wasm PRIVATE Eigen3::Eigen)
    set_target_properties(scalatrix_wasm PROPERTIES
        OUTPUT_NAME "scalatrix"
        SUFFIX ".js"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/wasm
        LINK_FLAGS "--bind -s MODULARIZE=1 -s EXPORT_NAME='Scalatrix' -s USE_WEBGL2=1 -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]' -s SINGLE_FILE=1"
    )
endif()

if (NOT EMSCRIPTEN)
    add_library(scalatrix_alias ALIAS scalatrix)
endif()

add_custom_target(native
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target scalatrix
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/native
)

add_custom_target(wasm
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/wasm --target scalatrix_wasm
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/wasm
)

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/wasm --target clean
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(build_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target scalatrix
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/wasm --target scalatrix_wasm
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)